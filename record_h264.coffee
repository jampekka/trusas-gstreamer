#!/usr/bin/env coffee

yargs = require 'yargs'
subprocess = require 'child_process'
Path = require 'path'

record = (opts) ->
	pipeline = """
	gst-launch-1.0 -q
		v4l2src device="#{opts.video}" do-timestamp=true ! video/x-h264,framerate=30/1,width=1280,height=720 ! h264parse ! tee name=src
		mpegtsmux name=mux ! fdsink fd=1
		src. ! queue ! mux.
		src. ! queue ! abstime ! text/x-raw ! capssetter join=false replace=true caps=meta/x-klv,parsed=true ! mux. 
	"""
	pipeline = pipeline.replace /\n/g, " "
	console.warn pipeline
	pp = [process.env.GST_PLUGIN_PATH, Path.join __dirname, 'gstabstime', 'src'].join(':')
	env = Object.assign process.env, GST_PLUGIN_PATH: pp
	subprocess.spawn pipeline, [],
		shell: true
		stdio: ['inherit', 'inherit', 'inherit']
		env: env


if module == require.main
	opts = yargs
		.option 'video', alias: 'v', describe: 'video device'
		.option 'format',
			alias: 'f',
			describe: 'gstreamer caps string'
		.demandOption(['video'])
		.help()
		.argv
	record opts

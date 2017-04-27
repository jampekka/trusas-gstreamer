# Note: plugindir is set in configure

##############################################################################
# TODO: change libgstplugin.la to something else, e.g. libmysomething.la     #
##############################################################################
plugin_LTLIBRARIES = libgstabstime.la

##############################################################################
# TODO: for the next set of variables, name the prefix if you named the .la, #
#  e.g. libmysomething.la => libmysomething_la_SOURCES                       #
#                            libmysomething_la_CFLAGS                        #
#                            libmysomething_la_LIBADD                        #
#                            libmysomething_la_LDFLAGS                       #
##############################################################################

## GstAbsTime

# sources used to compile this plug-in
libgstabstime_la_SOURCES = gstabstime.c

# compiler and linker flags used to compile this plugin, set in configure.ac
libgstabstime_la_CFLAGS = $(GST_CFLAGS)
libgstabstime_la_LIBADD = $(GST_LIBS)
libgstabstime_la_LDFLAGS = $(GST_PLUGIN_LDFLAGS)
libgstabstime_la_LIBTOOLFLAGS = --tag=disable-static


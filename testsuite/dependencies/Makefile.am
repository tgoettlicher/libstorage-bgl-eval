#
# Makefile.am for libstorage/testsuite/dependencies
#

AM_CPPFLAGS = -I$(top_srcdir)

LDADD = ../../storage/libstorage.la ../helpers/libhelpers.la			\
	-lboost_unit_test_framework

check_PROGRAMS =								\
	test1.test test2.test test3.test test4.test

AM_DEFAULT_SOURCE_EXT = .cc

TESTS = $(check_PROGRAMS)

EXTRA_DIST =									\
	test1-probed.xml test1-staging.xml 					\
	test3-probed.xml test3-staging.xml test3-expected.txt			\
	test4-probed.xml test4-staging.xml test4-expected.txt


#!/bin/sh

############################################################
# Set list of source directories to which symlinks should
# be created to
############################################################
SRCDIRS="FileStager Root bin"

echo "Bootstrapping..."

############################################################
# Check requirements
############################################################
if [ -z `which root-config` ]; then
    echo "Could not find root-config in PATH"
    exit 1
fi

############################################################
# Setup sed
############################################################
if [ x`uname -s` = xDarwin ]; then
    SED_VERSION=`sed --version 2>&1 | head -n1`
    if echo $SED_VERSION | grep -q illegal; then
	echo -n "Default sed is too old, looking for gsed..."
	if [ x`which gsed` = x ]; then
	    echo "not found"
	    echo "Please install MacPorts and run 'sudo port install gsed'"
	    exit 1
	else
	    echo "found"
	    SED_PROGRAM=gsed
	fi
    else
	SED_PROGRAM=sed
    fi
else
    SED_PROGRAM=sed
fi

if [ x`which $SED_PROGRAM` = x ]; then
    echo "sed is not installed, bailing out"
    exit 1
fi

export SED_PROGRAM

############################################################
# Create symlinks to source directories
############################################################
for dir in $SRCDIRS; do
    ln -sf ../$dir
done

############################################################
# Create files required by automake
############################################################
cp ../ChangeLog .

touch AUTHORS

############################################################
# Start build
############################################################
mkdir -p m4

# Generate Makefile.am
cat > Makefile.am <<EOF
ACLOCAL_AMFLAGS = -I m4

# set some variables by hand for old autoconf versions
builddir = .
abs_srcdir = \`cd \$(srcdir) && pwd\`
SED = $SED_PROGRAM

lib_LTLIBRARIES = libFileStager.la

libFileStager_la_CXXFLAGS = \$(ROOT_CFLAGS) -D__FSSTANDALONE__ -I\$(builddir)/Root

libFileStager_la_LDFLAGS = \$(ROOT_LDFLAGS) \$(ROOT_LIBS)

EXTRA_DIST = Root/LinkDef.h

bin_PROGRAMS = StageMonitor

StageMonitor_CXXFLAGS = \$(ROOT_CFLAGS) -D__FSSTANDALONE__

StageMonitor_LDFLAGS = \$(ROOT_LDFLAGS) \$(ROOT_LIBS)

StageMonitor_LDADD = \$(builddir)/libFileStager.la

nodist_libFileStager_la_SOURCES = \$(builddir)/Root/FileStagerCint.cxx

EOF

./generate_filelists.sh >> Makefile.am

cat >> Makefile.am <<EOF
\$(builddir)/Root/FileStagerCint.cxx:
	mkdir -p \$(builddir)/Root
	rootcint -f \$(builddir)/Root/FileStagerCint.cxx -c -p -I\$(abs_srcdir) \$(ROOT_CFLAGS) \$(abs_srcdir)/FileStager/T*.h \$(abs_srcdir)/Root/LinkDef.h

clean-local:
	-rm -f \$(builddir)/Root/FileStagerCint.h
	-rm -f \$(builddir)/Root/FileStagerCint.cxx

# Create config script
#

PACKAGEPREFIX = \$(prefix)
PACKAGENAME = \$(PACKAGE)
PACKAGEVERSION = \$(VERSION)
PACKAGELTLIBS = \$(libdir)/libFileStager.la
PACKAGECFLAGS = -I\$(includedir)
PACKAGEEXTCFLAGS = \$(CPPFLAGS)
PACKAGELDFLAGS = -L\$(libdir) -lFileStager
PACKAGELDADD = ""
PACKAGELIBS = -lFileStager
PACKAGESTATICLIBS = \$(libdir)/libFileStager.a

# From here on, no changes need to be applied
#

gen_config = \$(SED) \\
	-e 's,_PACKAGEPREFIX_,\$(PACKAGEPREFIX),g' \\
	-e 's,_PACKAGENAME_,\$(PACKAGENAME),g' \\
	-e 's,_PACKAGEVERSION_,\$(PACKAGEVERSION),g' \\
	-e 's,_PACKAGELTLIBS_,\$(PACKAGELTLIBS),g' \\
	-e 's,_PACKAGECFLAGS_,\$(PACKAGECFLAGS),g' \\
	-e 's,_PACKAGEEXTCFLAGS_,\$(PACKAGEEXTCFLAGS),g' \\
	-e 's,_PACKAGELDFLAGS_,\$(PACKAGELDFLAGS),g' \\
	-e 's,_PACKAGELDADD_,\$(PACKAGELDADD),g' \\
	-e 's,_PACKAGELIBS_,\$(PACKAGELIBS),g' \\
	-e 's,_PACKAGESTATICLIBS_,\$(PACKAGESTATICLIBS),g'

\$(PACKAGENAME)-config: package-config.in Makefile
	\$(gen_config) \$< > \$@
	chmod +x \$@

all: \$(PACKAGENAME)-config

install-exec-local: all
	\$(mkinstalldirs) \$(DESTDIR)\$(bindir)
	\$(INSTALL) \$(PACKAGENAME)-config \$(DESTDIR)\$(bindir)

uninstall-local:
	rm -f \$(DESTDIR)\$(bindir)/\$(PACKAGENAME)-config
	-rmdir \$(DESTDIR)\$(bindir)

MOSTLYCLEANFILES = \$(PACKAGENAME)-config

EXTRA_DIST += package-config.in
EOF

autoreconf -fiv

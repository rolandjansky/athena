%global __os_install_post /usr/lib/rpm/brp-compress %{nil}

%{!?_version: %{error:_version not defined}}
%{!?_release: %{expand:%define _release 1}}

%{!?_swversion: %{error:_swversion not defined}}

Name:           DBRelease-%{_version}
Version:        %(echo %{_version} | %{__sed} 's/-/./g')
Release:        %{_release}
Epoch:          0
Summary:        DBRelease

Group:          Application
License:        ATLAS
URL:            http://atlas.web.cern.ch/Atlas/GROUPS/DATABASE/pacman4/DBRelease/%{name}.tar.gz
Packager:       ATLAS Offline
Source0:        %{name}.tar.gz
BuildRoot:      %{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)

BuildArch:      noarch

AutoReqProv:    no
#Requires:       AtlasLogin

Prefix:         /opt/atlas

%description
Package DBRelease for s/w rel %{_swversion}

%prep

%build

%define _fixowner  [ `%{__id_u}` = '0' ] && %{__chown_Rhf} root
%define _fixgroup  [ `%{__id_u}` = '0' ] && %{__chgrp_Rhf} root

%install
%{__rm} -rf $RPM_BUILD_ROOT
%{__mkdir_p} $RPM_BUILD_ROOT%{prefix}
cd $RPM_BUILD_ROOT%{prefix}
%{__gzip} -dc %{SOURCE0} | %{__tar} -xvvf -
if [ -h DBRelease/current ]; then
    %{__rm} -f DBRelease/current
fi
STATUS=$?
if [ $STATUS -ne 0 ]; then
  exit $STATUS
fi
%{_fixowner} .
%{_fixgroup} .
%{_fixperms} .

%clean
%{__rm} -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root,-)
%{prefix}/DBRelease/%{_version}

%post
{
dbdir=$RPM_INSTALL_PREFIX/DBRelease/%{_version}
# Edit XML configuration files, insert the full path to SQLite replicas
fn=dblookup.xml
fp=$dbdir/XMLConfig/$fn
sed -i "s#sqliteGenerated:#sqlite_file:$dbdir/#" $fp
# Edit file catalogs, insert the full paths to CondDB payload files
fns="PoolCat_oflcond.xml PoolCat_tbcond.xml PoolCat_cmccond.xml \
PoolCat_comcond.xml"
fps=
for fn in $fns; do
  fps="$fps $dbdir/poolcond/$fn"
done
sed -i "s#--GENERATED--#$dbdir/poolcond/#" $fps
# Modify the installed kit
#echo "Updating $RPM_INSTALL_PREFIX/cmtsite/requirements..."
#sed -i.dborig '$a\
#set DBRELEASE_INSTALLED %{_version}
#/DBRELEASE/d' \
#$RPM_INSTALL_PREFIX/cmtsite/requirements

if [ -h $RPM_INSTALL_PREFIX/DBRelease/current ]; then
    rm -f $RPM_INSTALL_PREFIX/DBRelease/current
fi
ln -s %{_version} $RPM_INSTALL_PREFIX/DBRelease/current
} >>$RPM_INSTALL_PREFIX/var/log/scripts.log 2>&1

%changelog
* Thu Dec 06 2012 Grigori Rybkine <Grigori.Rybkine@cern.ch>
- Override system __os_install_post macro
- Define _fixowner, _fixgroup macros since they are missing with RPM 4.8.0 and later
- In %%install section, do not exit (so as to let RPM run __spec_install_post actions)

* Tue Aug 08 2007 Grigori Rybkine <Grigori.Rybkine@cern.ch>
- Make spec file more generic by introducing the _version, _release, _swversion macros
- Prints message to stderr and return BADSPEC if the _version or _swversion macro
  not defined on the command line
- Set the _release macro to 1 if not defined on the command line
- Drop dependency on AtlasLogin (starting from DBRelease 4.1.1)

* Sun Jul 01 2007 Grigori Rybkine <Grigori.Rybkine@cern.ch>
- Make spec file more generic by introducing the dbversion, swversion parameters
- Redirect the post scriptlet stdout and stderr to a log file

* Mon Jan 22 2007 Grigori Rybkine <Grigori.Rybkine@cern.ch>
- Update to version 3.1.1

* Sun Dec 17 2006 Grigori Rybkine <Grigori.Rybkine@cern.ch>
- Update to version 3.0.1

* Sat Dec 02 2006 Grigori Rybkine <Grigori.Rybkine@cern.ch>
- Update to version 2.9.1

* Fri Dec 01 2006 Grigori Rybkine <Grigori.Rybkine@cern.ch>
- Update to new version (3.0-LST)
- Modify the post scriptlet

* Sat Oct 21 2006 Grigori Rybkine <Grigori.Rybkine@rhul.ac.uk>
- Modify the post scriptlet (2.8-2)

* Sun Oct 15 2006 Grigori Rybkine <Grigori.Rybkine@rhul.ac.uk>
- First spec file for DBRelease (2.8-1)

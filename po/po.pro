TEMPLATE = subdirs

PROJECTNAME = "ubuntu-system-settings"

DESKTOPFILE = ../$${PROJECTNAME}.desktop

SOURCECODE = ../plugins/*/*.qml \
             ../src/qml/*.qml

BUILDDIR = ../.build
DESKTOPFILETEMP = $${BUILDDIR}/$${PROJECTNAME}.desktop.js

message("")
message(" Project Name: $$PROJECTNAME ")
message(" Source Code: $$SOURCECODE ")
message("")
message(" Run 'make pot' to generate the pot file from source code. ")
message("")

## Generate pot file 'make pot'
potfile.target = pot
potfile.commands = xgettext \
                   -o $${PROJECTNAME}.pot \
		   --copyright=\"Canonical Ltd. \" \
		   --package-name $${PROJECTNAME} \
		   --qt --c++ --add-comments=TRANSLATORS \
		   --keyword=tr --keyword=tr:1,2 --from-code=UTF-8 \
		   $${SOURCECODE} $${DESKTOPFILETEMP}
potfile.depends = desktopfile
QMAKE_EXTRA_TARGETS += potfile

## Do not use this rule directly. It's a dependency rule to
## generate an intermediate .h file to extract translatable
## strings from the .desktop file
desktopfile.target = desktopfile
desktopfile.commands = awk \'BEGIN { FS=\"=\" }; /Name/ {print \"var s = i18n.tr(\42\" \$$2 \"\42);\"}\' $${DESKTOPFILE} > $${DESKTOPFILETEMP}
desktopfile.depends = makebuilddir
QMAKE_EXTRA_TARGETS += desktopfile

## Dependency rule to create the temporary build dir
makebuilddir.target = makebuilddir
makebuilddir.commands = mkdir -p $${BUILDDIR}
QMAKE_EXTRA_TARGETS += makebuilddir

PO_FILES = $$system(ls *.po)

## Install the translations
install.target = install
install_mo_commands =
for(po_file, PO_FILES) {
mo_name = $$replace(po_file,.po,)
mo_targetpath = $(INSTALL_ROOT)/usr/share/locale/$${mo_name}/LC_MESSAGES
mo_target = $${mo_targetpath}/$${PROJECTNAME}.mo
!isEmpty(install_mo_commands): install_mo_commands += &&
install_mo_commands += test -d $$mo_targetpath || mkdir -p $$mo_targetpath
install_mo_commands += && cp $${mo_name}.mo $$mo_target
}
install.commands = $$install_mo_commands
install.depends = mofiles
QMAKE_EXTRA_TARGETS += install

## Build $locale.mo from the $locale.po files (called by the installed rule)
mofiles.target = mofiles
mofiles_po_commands =
for(po_file, PO_FILES) {
po_name = $$replace(po_file,.po,)
install_po_commands += msgfmt $$po_file -o $${po_name}.mo;
}
mofiles.commands = $$install_po_commands
QMAKE_EXTRA_TARGETS += mofiles

## Rule to clean the products of the build
clean.target = clean
clean.commands = rm -Rf $${BUILDDIR} *.mo
QMAKE_EXTRA_TARGETS += clean
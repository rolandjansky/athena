atlas_subdir( Template )

# Component(s) in the package:
atlas_add_root_dictionary (TemplateLib
   TemplateDictSource
   ROOT_HEADERS Template/*.h Root/LinkDef.h
   EXTERNAL_PACKAGES ROOT )

# add the library itself:
atlas_add_library (TemplateLib
   Template/*.h Template/*.ihh Root/*.cxx ${TemplateDictSource}
   PUBLIC_HEADERS Template
   INCLUDE_DIRS 
   LINK_LIBRARIES EventLoop xAODEventInfo xAODTracking xAODTruth
   PRIVATE_LINK_LIBRARIES )

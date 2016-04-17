# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

######################################################
# XML Dumper module
#
# 
#
######################################################

class XMLDumper():
    def __init__(self, name = 'InDetXMLDumper', fileName = 'test.xml'):
        self.out_file = open(fileName,"w")
        self.pixel_builders = []
        self.sct_builders = []
        self.trt_builders = []

    def addPixelBuilder(self, builder):
        self.pixel_builders.append(builder)

    def addSCTBuilder(self, builder):
        self.sct_builders.append(builder)
        
    def addTRTBuilder(self, builder):
        self.trt_builders.append(builder)
        
    def dump(self):
    #, IBLLayerBuilder, PixelLayerBuilder, SCT_LayerBuilder, TRT_LayerBuilder):
        self.out_file.write("<!--\n The InnerDetector identifier is arranged as follows \n \
\tInnerDetector\n \
\t\tpart=Pixel\n \
\t\t\tside=barrel\n \
\t\t\t\tlayer phi_module eta_module phi_index eta_index\n \
\t\t\tside=endcap\n \
\t\t\t\tdisk phi_module eta_module phi_index eta_index\n \
\t\tpart=SCT\n \
\t\t\tside=barrel\n \
\t\t\t\tlayer phi_module eta_module side strip\n \
\t\t\tside=endcap\n \
\t\t\t\t\tdisk phi_module eta_module side strip\n \
\t\tpart=TRT\n \
\t\t\tside=barrel\n \
\t\t\t\tlayer phi_module straw_layer straw\n \
\t\t\tside=endcap\n \
\t\t\t\twheel phi_module straw_layer straw\n -->\n")
        
        self.out_file.write('<IdDictionary name="InnerDetector" >\n')
        self.out_file.write('  <field name="part" >\n')
        self.out_file.write('    <label name="Pixel" value="1" />\n')
        self.out_file.write('    <label name="SCT" value="2" />\n')
        self.out_file.write('    <label name="TRT" value="3" />\n')
        self.out_file.write('  </field>\n')

        self.out_file.write('  <field name="barrel_endcap">\n')
        self.out_file.write('    <label name="negative_endcap" value="-2" />\n')
        self.out_file.write('    <label name="negative_barrel" value="-1" />\n')
        self.out_file.write('    <label name="barrel"          value="0" />\n')
        self.out_file.write('    <label name="positive_barrel" value="+1"  />\n')
        self.out_file.write('    <label name="positive_endcap" value="+2"  />\n')
        self.out_file.write('  </field>\n')

        layer_offset = 0
        disk_offset = 0

        for pix_builder in self.pixel_builders:
            for layer in range(pix_builder.BarrelLayers):
                self.out_file.write('  <region group="pixel" >\n')
                self.out_file.write('    <range field="part" value="'+pix_builder.Identification+'" />\n')
                self.out_file.write('    <range field="barrel_endcap" value="barrel" />\n')
                self.out_file.write('    <range field="layer" value="'+str(layer_offset+layer)+'" />\n')
                times = 1
                if pix_builder.LayerSCTlike:
                    times = 2
                self.out_file.write('    <range field="phi_module" minvalue="0" maxvalue="'+
                                    str(pix_builder.LayerPhiSectors[layer]*times-1)+'" wraparound="TRUE" />\n')
                self.out_file.write('    <range field="eta_module" minvalue="-'+
                                    str((pix_builder.LayersZsectors[layer])/2)+'" maxvalue="'+
                                    str((pix_builder.LayersZsectors[layer])/2)+'" />\n')
                self.out_file.write('    <range field="phi_index" minvalue="0" maxvalue="'+
                                    str(int(pix_builder.LayerLengthXmin[layer]/pix_builder.LayerPitchX[layer]))+'" />\n')
                self.out_file.write('    <range field="eta_index" minvalue="0" maxvalue="'+
                                    str(int(pix_builder.LayerLengthY[layer]/pix_builder.LayerPitchY[layer]))+'" />\n')
                self.out_file.write('  </region>\n')

            layer_offset += pix_builder.BarrelLayers

            
            for disk in range(pix_builder.EndcapDiscs):
                for ring in range(len(pix_builder.DiscPhiSectors[disk])):
                    iring = len(pix_builder.DiscPhiSectors[disk])-ring-1
                    self.out_file.write('  <region group="pixel" >\n')
                    self.out_file.write('    <range field="part" value="Pixel" />\n')
                    self.out_file.write('    <range field="barrel_endcap" values="negative_endcap positive_endcap" />\n')
                    self.out_file.write('    <range field="disk" value="'+str(disk_offset+disk)+'" />\n')
                    self.out_file.write('    <range field="phi_module" minvalue="0" maxvalue="'+
                                        str(pix_builder.DiscPhiSectors[disk][iring]-1)+'" wraparound="TRUE" />\n')
                    self.out_file.write('    <range field="eta_module" value="'+str(ring)+'" />\n')
                    self.out_file.write('    <range field="phi_index" minvalue="0" maxvalue="'+
                                        str(int(pix_builder.DiscLengthXmin[disk][iring]/pix_builder.DiscPitchX[disk][iring]))+'" />\n')
                    self.out_file.write('    <range field="eta_index" minvalue="0" maxvalue="'+
                                        str(int(pix_builder.DiscLengthY[disk][iring]/pix_builder.DiscPitchY[disk][iring]))+'" />\n')
                    self.out_file.write('  </region>\n')
            
            disk_offset  += pix_builder.EndcapDiscs
       
        layer_offset = 0
        disk_offset = 0

        for sct_builder in self.sct_builders:
            for layer in range(sct_builder.BarrelLayers):
                self.out_file.write('  <region group="sct" >\n')
                self.out_file.write('    <range field="part" value="'+sct_builder.Identification+'" />\n')
                self.out_file.write('    <range field="barrel_endcap" value="barrel" />\n')
                self.out_file.write('    <range field="layer" value="'+str(layer_offset+layer)+'" />\n')
                self.out_file.write('    <range field="phi_module" minvalue="0" maxvalue="'+
                                    str(sct_builder.LayerPhiSectors[layer]-1)+'" wraparound="TRUE" />\n')
                
                min_val = -(sct_builder.LayersZsectors[layer]/2)
                max_val = sct_builder.LayersZsectors[layer]/2
                values = range(min_val, max_val)
                
                if not sct_builder.LayersZsectors[layer]%2:
                    values.remove(0)
                    self.out_file.write('    <range field="eta_module" values="')
                for value in values:
                    self.out_file.write(str(value)+' ')
                self.out_file.write(str((sct_builder.LayersZsectors[layer])/2)+'" />\n')
                self.out_file.write('    <range field="side" minvalue="0" maxvalue="1" />\n')
                self.out_file.write('    <range field="strip" minvalue="0" maxvalue="'+
                                    str(int(sct_builder.LayerLengthXmin[layer]/sct_builder.LayerPitchX[layer]))+'" />\n')
                self.out_file.write('  </region>\n')

            layer_offset += sct_builder.BarrelLayers
            
            for disk in range(sct_builder.EndcapDiscs):
                for ring in range(len(sct_builder.DiscPhiSectors[disk])):
                    iring = len(sct_builder.DiscPhiSectors[disk])-ring-1
                    self.out_file.write('  <region group="sct" >\n')
                    self.out_file.write('    <range field="part" value="'+sct_builder.Identification+'" />\n')
                    self.out_file.write('    <range field="barrel_endcap" values="negative_endcap positive_endcap" />\n')
                    self.out_file.write('    <range field="disk" value="'+str(disk_offset+disk)+'" />\n')
                    self.out_file.write('    <range field="phi_module" minvalue="0" maxvalue="'+
                                        str(sct_builder.DiscPhiSectors[disk][iring]-1)+'" wraparound="TRUE" />\n')
                    self.out_file.write('    <range field="eta_module" value="'+str(ring)+'" />\n')
                    self.out_file.write('    <range field="side" minvalue="0" maxvalue="1" />\n')
                    import math
                    phisec = 2.*math.asin(sct_builder.DiscLengthXmax[disk][iring]/2./sct_builder.DiscRingMaxR[disk][iring])
                    self.out_file.write('    <range field="strip" minvalue="0" maxvalue="'+
                                        str(int(phisec/sct_builder.DiscPitchX[disk][iring]))+'" />\n')
                    self.out_file.write('  </region>\n')

            disk_offset  += sct_builder.EndcapDiscs
            
        for trt_builder in self.trt_builders:

            self.out_file.write('  <subregion name="TRT_barrel">\n')
            self.out_file.write('    <range field="part" value="TRT" />\n')
            self.out_file.write('    <range field="barrel_endcap" values="negative_barrel positive_barrel" />\n')
            self.out_file.write('  </subregion>\n')
            
            self.out_file.write('  <subregion name="TRT_endcap">\n')
            self.out_file.write('    <range field="part" value="TRT" />\n')
            self.out_file.write('    <range field="barrel_endcap" values="negative_endcap positive_endcap" />\n')
            self.out_file.write('  </subregion>\n')
    
            self.out_file.write('  <subregion name="TRT_layer_0">\n')
            self.out_file.write('    <reference subregion="TRT_barrel" />\n')
            self.out_file.write('    <range field="phi_sector" minvalue="0" maxvalue="31" wraparound="TRUE" />\n')
            self.out_file.write('    <range field="layer_or_wheel" value="0" />\n')
            self.out_file.write('  </subregion>\n')
            
            self.out_file.write('  <region group="trt" >\n')
            self.out_file.write('    <reference subregion="TRT_layer_0" />\n')
            self.out_file.write('    <range field="straw_layer" value="0" />\n')
            self.out_file.write('    <range field="straw" minvalue="0" maxvalue="14" />\n')
            self.out_file.write('  </region>\n')
            
            self.out_file.write('  <region group="trt" >\n')
            self.out_file.write('    <reference subregion="TRT_layer_0" />\n')
            self.out_file.write('    <range field="straw_layer" minvalue="1" maxvalue="4" />\n')
            self.out_file.write('    <range field="straw" minvalue="0" maxvalue="15" />\n')
            self.out_file.write('  </region>\n')
            
            self.out_file.write('  <region group="trt" >\n')
            self.out_file.write('    <reference subregion="TRT_layer_0" />\n')
            self.out_file.write('    <range field="straw_layer" minvalue="5" maxvalue="9" />\n')
            self.out_file.write('    <range field="straw" minvalue="0" maxvalue="16" />\n')
            self.out_file.write('  </region>\n')
            
            self.out_file.write('  <region group="trt" >\n')
            self.out_file.write('    <reference subregion="TRT_layer_0" />\n')
            self.out_file.write('    <range field="straw_layer" minvalue="10" maxvalue="14" />\n')
            self.out_file.write('    <range field="straw" minvalue="0" maxvalue="17" />\n')
            self.out_file.write('  </region>\n')
            
            self.out_file.write('  <region group="trt" >\n')
            self.out_file.write('    <reference subregion="TRT_layer_0" />\n')
            self.out_file.write('    <range field="straw_layer" minvalue="15" maxvalue="17" />\n')
            self.out_file.write('    <range field="straw" minvalue="0" maxvalue="18" />\n')
            self.out_file.write('  </region>\n')
            
            self.out_file.write('  <region group="trt" >\n')
            self.out_file.write('    <reference subregion="TRT_layer_0" />\n')
            self.out_file.write('    <range field="straw_layer" value="18" />\n')
            self.out_file.write('    <range field="straw" minvalue="0" maxvalue="17" />\n')
            self.out_file.write('  </region>\n')

            self.out_file.write('  <subregion name="TRT_layer_1">\n')
            self.out_file.write('    <reference subregion="TRT_barrel" />\n')
            self.out_file.write('    <range field="phi_sector" minvalue="0" maxvalue="31" wraparound="TRUE" />\n')
            self.out_file.write('    <range field="layer_or_wheel" value="1" />\n')
            self.out_file.write('  </subregion>\n')
            
            self.out_file.write('  <region group="trt" >\n')
            self.out_file.write('    <reference subregion="TRT_layer_1" />\n')
            self.out_file.write('    <range field="straw_layer" value="0" />\n')
            self.out_file.write('    <range field="straw" minvalue="0" maxvalue="18" />\n')
            self.out_file.write('  </region>\n')

            self.out_file.write('  <region group="trt" >\n')
            self.out_file.write('    <reference subregion="TRT_layer_1" />\n')
            self.out_file.write('    <range field="straw_layer" minvalue="1" maxvalue="5" />\n')
            self.out_file.write('    <range field="straw" minvalue="0" maxvalue="19" />\n')
            self.out_file.write('  </region>\n')
            
            self.out_file.write('  <region group="trt" >\n')
            self.out_file.write('    <reference subregion="TRT_layer_1" />\n')
            self.out_file.write('    <range field="straw_layer" minvalue="6" maxvalue="10" />\n')
            self.out_file.write('    <range field="straw" minvalue="0" maxvalue="20" />\n')
            self.out_file.write('  </region>\n')
            
            self.out_file.write('  <region group="trt" >\n')
            self.out_file.write('    <reference subregion="TRT_layer_1" />\n')
            self.out_file.write('    <range field="straw_layer" minvalue="11" maxvalue="15" />\n')
            self.out_file.write('    <range field="straw" minvalue="0" maxvalue="21" />\n')
            self.out_file.write('  </region>\n')
            
            self.out_file.write('  <region group="trt" >\n')
            self.out_file.write('    <reference subregion="TRT_layer_1" />\n')
            self.out_file.write('    <range field="straw_layer" minvalue="16" maxvalue="20" />\n')
            self.out_file.write('    <range field="straw" minvalue="0" maxvalue="22" />\n')
            self.out_file.write('  </region>\n')
            
            self.out_file.write('  <region group="trt" >\n')
            self.out_file.write('    <reference subregion="TRT_layer_1" />\n')
            self.out_file.write('    <range field="straw_layer" minvalue="21" maxvalue="22" />\n')
            self.out_file.write('    <range field="straw" minvalue="0" maxvalue="23" />\n')
            self.out_file.write('  </region>\n')
            
            self.out_file.write('  <region group="trt" >\n')
            self.out_file.write('    <reference subregion="TRT_layer_1" />\n')
            self.out_file.write('    <range field="straw_layer" value="23" />\n')
            self.out_file.write('    <range field="straw" minvalue="0" maxvalue="22" />\n')
            self.out_file.write('  </region>\n')

            self.out_file.write('  <subregion name="TRT_layer_2">\n')
            self.out_file.write('    <reference subregion="TRT_barrel" />\n')
            self.out_file.write('    <range field="phi_sector" minvalue="0" maxvalue="31" wraparound="TRUE" />\n')
            self.out_file.write('    <range field="layer_or_wheel" value="2" />\n')
            self.out_file.write('  </subregion>\n')
            
            self.out_file.write('  <region group="trt" >\n')
            self.out_file.write('    <reference subregion="TRT_layer_2" />\n')
            self.out_file.write('    <range field="straw_layer" value="0" />\n')
            self.out_file.write('    <range field="straw" minvalue="0" maxvalue="22" />\n')
            self.out_file.write('  </region>\n')

            self.out_file.write('  <region group="trt" >\n')
            self.out_file.write('    <reference subregion="TRT_layer_2" />\n')
            self.out_file.write('    <range field="straw_layer" minvalue="1" maxvalue="4" />\n')
            self.out_file.write('    <range field="straw" minvalue="0" maxvalue="23" />\n')
            self.out_file.write('  </region>\n')
            
            self.out_file.write('  <region group="trt" >\n')
            self.out_file.write('    <reference subregion="TRT_layer_2" />\n')
            self.out_file.write('    <range field="straw_layer" minvalue="5" maxvalue="9" />\n')
            self.out_file.write('    <range field="straw" minvalue="0" maxvalue="24" />\n')
            self.out_file.write('  </region>\n')
            
            self.out_file.write('  <region group="trt" >\n')
            self.out_file.write('    <reference subregion="TRT_layer_2" />\n')
            self.out_file.write('    <range field="straw_layer" minvalue="10" maxvalue="14" />\n')
            self.out_file.write('    <range field="straw" minvalue="0" maxvalue="25" />\n')
            self.out_file.write('  </region>\n')
            
            self.out_file.write('  <region group="trt" >\n')
            self.out_file.write('    <reference subregion="TRT_layer_2" />\n')
            self.out_file.write('    <range field="straw_layer" minvalue="15" maxvalue="19" />\n')
            self.out_file.write('    <range field="straw" minvalue="0" maxvalue="26" />\n')
            self.out_file.write('  </region>\n')
            
            self.out_file.write('  <region group="trt" >\n')
            self.out_file.write('    <reference subregion="TRT_layer_2" />\n')
            self.out_file.write('    <range field="straw_layer" minvalue="20" maxvalue="24" />\n')
            self.out_file.write('    <range field="straw" minvalue="0" maxvalue="27" />\n')
            self.out_file.write('  </region>\n')
            
            self.out_file.write('  <region group="trt" >\n')
            self.out_file.write('    <reference subregion="TRT_layer_2" />\n')
            self.out_file.write('    <range field="straw_layer" minvalue="25" maxvalue="28" />\n')
            self.out_file.write('    <range field="straw" minvalue="0" maxvalue="28" />\n')
            self.out_file.write('  </region>\n')
            
            self.out_file.write('  <region group="trt" >\n')
            self.out_file.write('    <reference subregion="TRT_layer_2" />\n')
            self.out_file.write('    <range field="straw_layer" value="29" />\n')
            self.out_file.write('    <range field="straw" minvalue="0" maxvalue="27" />\n')
            self.out_file.write('  </region>\n')

            self.out_file.write('  <region group="trt" >\n')
            self.out_file.write('    <reference subregion="TRT_endcap" />\n')
            self.out_file.write('    <range field="phi_sector" minvalue="0" maxvalue="31" wraparound="TRUE" />\n')
            self.out_file.write('    <range field="layer_or_wheel" minvalue="0" maxvalue="5" />\n')
            self.out_file.write('    <range field="straw_layer" minvalue="0" maxvalue="15" />\n')
            self.out_file.write('    <range field="straw" minvalue="0" maxvalue="23" />\n')
            self.out_file.write('  </region>\n')
            
            self.out_file.write('  <region group="trt" >\n')
            self.out_file.write('    <reference subregion="TRT_endcap" />\n')
            self.out_file.write('    <range field="phi_sector" minvalue="0" maxvalue="31" wraparound="TRUE" />\n')
            self.out_file.write('    <range field="layer_or_wheel" minvalue="6" maxvalue="13" />\n')
            self.out_file.write('    <range field="straw_layer" minvalue="0" maxvalue="7" />\n')
            self.out_file.write('    <range field="straw" minvalue="0" maxvalue="23" />\n')
            self.out_file.write('  </region>\n')
            
            self.out_file.write('  <alternate_regions>\n')
            self.out_file.write('    <region group="trt" >\n')
            self.out_file.write('      <reference subregion="TRT_endcap" />\n')
            self.out_file.write('      <range field="phi_sector" minvalue="0" maxvalue="31" wraparound="TRUE" />\n')
            self.out_file.write('      <range field="layer_or_wheel" minvalue="14" maxvalue="17" />\n')
            self.out_file.write('      <range field="straw_layer" minvalue="0" maxvalue="15" />\n')
            self.out_file.write('      <range field="straw" minvalue="0" maxvalue="17" />\n')
            self.out_file.write('    </region>\n')
            self.out_file.write('    <region group="trt"  tag="initial_layout" />\n')
            self.out_file.write('    <region group="trt" tag="final_layout" >\n')
            self.out_file.write('      <reference subregion="TRT_endcap" />\n')
            self.out_file.write('      <range field="phi_sector" minvalue="0" maxvalue="31" wraparound="TRUE" />\n')
            self.out_file.write('      <range field="layer_or_wheel" minvalue="14" maxvalue="17" />\n')
            self.out_file.write('      <range field="straw_layer" minvalue="0" maxvalue="15" />\n')
            self.out_file.write('      <range field="straw" minvalue="0" maxvalue="17" />\n')
            self.out_file.write('    </region>\n')
            self.out_file.write('    <region group="trt"  tag="destaged_layout" />\n')
            self.out_file.write('  </alternate_regions>\n')
            
            self.out_file.write('  <!-- Dummy region: This forces the bit packing to be the same for all layouts -->\n')
            
            self.out_file.write('  <region group="trt" name = "dummy">\n')
            self.out_file.write('    <range field="part" value="TRT" />\n')
            self.out_file.write('    <range field="barrel_endcap" values="negative_endcap negative_barrel positive_barrel positive_endcap"/>\n')
            self.out_file.write('    <range field="phi_sector" minvalue="0" maxvalue="31" />\n')
            self.out_file.write('    <range field="layer_or_wheel" minvalue="0" maxvalue="17" />\n')
            self.out_file.write('    <range field="straw_layer" minvalue="0" maxvalue="29" />\n')
            self.out_file.write('    <range field="straw" minvalue="0" maxvalue="28" />\n')
            self.out_file.write('  </region>\n')
                    
    def closeFile(self):
        self.out_file.write('</IdDictionary>\n')
        self.out_file.close()


    

        
        
        

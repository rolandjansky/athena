void set_vectors_of_collection_ids_and_size(std::vector<unsigned int> &element, std::vector<unsigned int> &nstraws, const unsigned int number_of_elements) {

  // The transient model has a container holding collections (TRT detector elements/modules)
  // and the collections hold channels (e.g. up to 24 straws in the TRT endcaps).
  // Here we construct, for the whole detector, the ids of the collections (14912 TRT detector elements/modules)
  // and the number of channels (straws) in each collection according to the numbering scheme in the TRT RDO container.
  // A total 350848 straw ids can be defined using these two vectors since the id of each straw simply follows from the
  // previous id starting from the id of the collection that contains it.

  /*
    5120 endcap elements : 32(6*(15*1,17),7*(7*1,25),7*1,601)
    4672 barrel elements : 64(18*1,14,23*1,9,29*1,931)
    5120 endcap elements : 32(6*(15*1,17),7*(7*1,25),7*1,601)
    ----
   14912  << number_of_elements
    ----
  */

  element.resize(number_of_elements);
  nstraws.resize(number_of_elements,24); // all endcap modules have 24 straws. The barrels will need a more detailed assignment.

  const unsigned int first_straw = 0x10000000u;
  unsigned int i,j,k,n;
  unsigned int c=0;

  n=0;
  element[n++]=0;
  for (i=0; i<32; i++) {
    for (j=0; j<6; j++) { for (k=0; k<15; k++) { c++; element[n++]=c; }; c=c+17; element[n++]=c; }
    for (j=0; j<7; j++) { for (k=0; k< 7; k++) { c++; element[n++]=c; }; c=c+25; element[n++]=c; }
    for (j=0; j<7; j++) { c++; element[n++]=c; }; c=c+601; element[n++]=c;
  }
  for (i=0; i<64; i++) {
    for (k=0; k<18; k++) { c++; element[n++]=c; }; c=c+14;  element[n++]=c;
    for (k=0; k<23; k++) { c++; element[n++]=c; }; c=c+9;   element[n++]=c;
    for (k=0; k<29; k++) { c++; element[n++]=c; }; c=c+931; element[n++]=c;
  }
  for (i=0; i<32; i++) {
    for (j=0; j<6; j++) { for (k=0; k<15; k++) { c++; element[n++]=c; }; c=c+17; element[n++]=c; }
    for (j=0; j<7; j++) { for (k=0; k< 7; k++) { c++; element[n++]=c; }; c=c+25; element[n++]=c; }
    for (j=0; j<7; j++) { c++; element[n++]=c; }; c=c+601; if (n<number_of_elements) element[n++]=c;
  } // the very last item in the pattern is dropped.

  // Finally convert from 22 bit to 32 bit form.
  for (i=0; i<number_of_elements; i++) element[i] = (element[i]<<10)+first_straw;

  /*
    5120 endcap elements : 24
    4672 barrel elements : 64(15,4*16,5*17,5*18,3*19,18,19,5*20,5*21,5*22,5*23,2*24,2*23,4*24,5*25,5*26,5*27,5*28,4*29,28)
    5120 endcap elements : 24
  */
  n=5120;
  for (i=0; i<64; i++) {
    nstraws[n++]=15;
    for (j=0; j<4; j++) nstraws[n++]=16;
    for (j=0; j<5; j++) nstraws[n++]=17;
    for (j=0; j<5; j++) nstraws[n++]=18;
    for (j=0; j<3; j++) nstraws[n++]=19;
    nstraws[n++]=18; nstraws[n++]=19;
    for (j=0; j<5; j++) nstraws[n++]=20;
    for (j=0; j<5; j++) nstraws[n++]=21;
    for (j=0; j<5; j++) nstraws[n++]=22;
    for (j=0; j<5; j++) nstraws[n++]=23;
    for (j=0; j<2; j++) nstraws[n++]=24;
    for (j=0; j<2; j++) nstraws[n++]=23;
    for (j=0; j<4; j++) nstraws[n++]=24;
    for (j=0; j<5; j++) nstraws[n++]=25;
    for (j=0; j<5; j++) nstraws[n++]=26;
    for (j=0; j<5; j++) nstraws[n++]=27;
    for (j=0; j<5; j++) nstraws[n++]=28;
    for (j=0; j<4; j++) nstraws[n++]=29;
    nstraws[n++]=28;
  }

/*
  // DEBUG dump out all straw ids (32-bit representation; no information is lost)
  unsigned int trt_strawid[350848];
  unsigned int istraw=0;
  for (unsigned int i=0; i<trt_number_of_collections; i++) {
    trt_strawid[istraw++] = trt_collection_id[i];
    for (unsigned int j=1; j<trt_collection_size[i]; j++) {
      trt_strawid[istraw]=trt_strawid[istraw-1]+0x20u;
      istraw++;
    }
  }
  std::cout << "AJB DUMP number of straws is " << istraw << std::endl;
  for (unsigned int k=0; k<350848; k++) std::cout << "AJB DUMP " << std::hex << trt_strawid[k] << std::endl;
*/

}

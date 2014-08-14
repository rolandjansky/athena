package atlantis.event;


import atlantis.output.ALogInterface;
import atlantis.output.AOutput;
import atlantis.parameters.APar;
import atlantis.data.AS3DData;


/**
 * Filter of S3D hits ask Hans Drevermann
 */

public class AFilter {

  final static int FILTER_GROUP=76;

  static int[] h=null;
  static int[] eta=null;
  static int[] phi=null;
  static int[] layer=null;
  static int[] group=null;
  static int[] skewGroup=null;
  static int[] groupMapping=null;

  static int[] bin=null;
  static int[] ll=null;
  static int[] hitLayers=null;
  static int[] id=null;
  static boolean[] good=null;
  static int numEtaBins=0;

  private static APar parameterStore = APar.instance();
  
  final static int[] layerAsBits=new int[32];
  static {
    layerAsBits[0]=1;
    for(int i=1; i<32; i++)
      layerAsBits[i]=2*layerAsBits[i-1];
  }


  public static void filter(AS3DData aS3DData) {
    if(aS3DData==null) {
      AOutput.append("AS3DData not found can't filter\n", ALogInterface.BAD_COMMAND);
      return;
    }

    int numUsefulPhiBins=parameterStore.get("Fil", "NumPhi").getI();
    int numUsefulEtaBins=parameterStore.get("Fil", "NumEta").getI();
    // +2 for wrap around and edge effect
    int numPhiBins=numUsefulPhiBins+2;

    // +2 for edge effects
    numEtaBins=numUsefulEtaBins+2;

    double etaRange=parameterStore.get("Fil", "EtaRange").getD();
    int requiredHitsInBin=parameterStore.get("Fil", "NumHits").getI();
    int numSkewSteps=parameterStore.get("Fil", "NumSkew").getI();
    double skewStepSize=parameterStore.get("Fil", "SkewStep").getD();

    int numHits=aS3DData.makeFilterDrawList(etaRange);
    int maxBins=numHits+1;

    if(h==null||h.length<numPhiBins*numEtaBins) h=new int[numPhiBins*numEtaBins];
    if(layer==null||layer.length<numHits) layer=new int[numHits];
    if(eta==null||eta.length<numHits) eta=new int[numHits];
    if(phi==null||phi.length<numHits) phi=new int[numHits];
    if(group==null||group.length<numHits) group=new int[numHits];
    if(skewGroup==null||skewGroup.length<numHits) skewGroup=new int[numHits];
    if(groupMapping==null||groupMapping.length<numHits) groupMapping=new int[numHits];
    //
    if(bin==null||bin.length<maxBins) bin=new int[maxBins];
    if(hitLayers==null||hitLayers.length<maxBins) hitLayers=new int[maxBins];
    if(id==null||id.length<maxBins) id=new int[maxBins];
    if(ll==null||ll.length<maxBins) ll=new int[maxBins];
    if(good==null||good.length<maxBins) good=new boolean[maxBins];

    layer=aS3DData.getLayer(layer);
    eta=aS3DData.getIntegerEta(numUsefulEtaBins, etaRange, eta);

    // initialise
    for(int i=0; i<numHits; ++i)
      group[i]=0;

    if(!parameterStore.get("Fil", "Loop").getStatus()) numSkewSteps=0;

    int numGroups=0;

    for(int step=-numSkewSteps; step<=numSkewSteps; step++) {
      double skew=0.;

      // TODO: Check if 166.666 should be the curvature parameter from the parameter store
      if(numSkewSteps!=0)
        skew=(1./skewStepSize)*step/(numSkewSteps*2*166.666);

      for(int i=0; i<maxBins; ++i) {
        hitLayers[i]=0;
        good[i]=false;
      }

      phi=aS3DData.getIntegerPhi(numUsefulPhiBins, skew, phi);

      int numBins=1;

      for(int i=0; i<numHits; ++i) {
        int b=phi[i]*numEtaBins+eta[i];

        if(h[b]==0) {
          h[b]=numBins;
          bin[numBins]=b;
          numBins++;
        }
        hitLayers[h[b]]|=layerAsBits[layer[i]];
      }

      // must treat the phi wraparound here......

      for(int e=0; e<numEtaBins; e++) {
        h[(0)*numEtaBins+e]=h[(numUsefulPhiBins)*numEtaBins+e];
        h[(numUsefulPhiBins+1)*numEtaBins+e]=h[(1)*numEtaBins+e];
      }

      int ngood=0;

      for(int i=1; i<numBins; i++) {
        int b=bin[i];
        int hitLayersSummed=0;

        // add hits from 3x3 region around this bin
        for(int p=b-numEtaBins; p<b+2*numEtaBins; p+=numEtaBins)
          for(int e=p-1; e<p+2; ++e) {
            int ibin=h[e];

            if(ibin>0)
              hitLayersSummed|=hitLayers[ibin];
          }
        if(countBits(hitLayersSummed)>=requiredHitsInBin) {
          good[i]=true;
          ll[ngood++]=i;
        }
      }

      for(int i=0; i<ngood; ++i)
        id[ll[i]]=0;

      int firstGroup=numGroups+1;

      for(int i=0; i<ngood; ++i)
        if(id[ll[i]]==0)
          add(ll[i], ++numGroups);

      for(int j=firstGroup; j<=numGroups; ++j) {
        int hls=0;

        for(int i=0; i<ngood; i++)
          if(id[ll[i]]==j) {
            hls|=hitLayers[ll[i]];
          }
        if(countBits(hls)<requiredHitsInBin)
          for(int i=0; i<ngood; i++)
            if(id[ll[i]]==j)
              good[ll[i]]=false;
      }

      for(int i=0; i<numHits; ++i) {
        int b=h[phi[i]*numEtaBins+eta[i]];

        if(good[b])
          skewGroup[i]=id[b];
        else
          skewGroup[i]=0;
      }

      // now zero the histogram for next call
      for(int i=1; i<numBins; i++)
        h[bin[i]]=0;
      // wrap around
      for(int e=0; e<numEtaBins; e++) {
        h[(0)*numEtaBins+e]=0;
        h[(numPhiBins-1)*numEtaBins+e]=0;
      }

      group=mergeGroups(numHits, numGroups, group, skewGroup);

    }

    group=renumberGroups(numHits, group);

    int num=0;

    for(int i=0; i<numHits; ++i)
      if(group[i]>0) num++;

    AOutput.alwaysAppend("\nFiltering:\n Input hits = "+numHits+"\n Num grouped = "+num+"\n",ALogInterface.NORMAL_BOLD);

    aS3DData.setGroup(group);
  }

  public static void add(int i, int numGroup) {
    id[i]=numGroup;
    int b=bin[i];

    // add hits from 3x3 region around this bin
    for(int p=b-numEtaBins; p<b+2*numEtaBins; p+=numEtaBins)
      for(int e=p-1; e<p+2; ++e) {
        int ibin=h[e];

        if(good[ibin]&&id[ibin]==0)
          add(ibin, numGroup);
      }
  }

  public static int[] mergeGroups(int numHits, int numGroups, int[] group, int[] skewgroup) {

    for(int i=1; i<=numGroups; ++i)
      groupMapping[i]=i;

    for(int i=0; i<numHits; ++i)
      if(skewGroup[i]>0&&group[i]>0) {
        int g1=groupMapping[group[i]];
        int g2=groupMapping[skewGroup[i]];

        if(g1!=g2)
          for(int g=1; g<=numGroups; ++g)
            if(groupMapping[g]==g1||groupMapping[g]==g2) groupMapping[g]=g1;
      }

    for(int i=0; i<numHits; ++i)
      if(skewGroup[i]>0||group[i]>0) {
        int g=skewGroup[i];

        if(g==0)
          g=group[i];
        group[i]=groupMapping[g];
      }
    return group;
  }

  public static int[] renumberGroups(int numHits, int[] group) {
    int numGroups=0;

    for(int i=0; i<numHits; ++i)
      if(group[i]>0) {
        int g=group[i];

        numGroups++;
        for(int j=i; j<numHits; ++j)
          if(group[j]==g)
            group[j]=-numGroups;
      }
    for(int i=0; i<numHits; ++i)
      group[i]=-group[i];
    return group;
  }

  // Return the number of bits currently set.  This is done via a lookup table

  final static int[] countLUT= {0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4, 1, 2, 2, 3, 2, 3, 3,
    4, 2, 3, 3, 4, 3, 4, 4, 5, 1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 2, 3, 3, 4, 3, 4, 4,
    5, 3, 4, 4, 5, 4, 5, 5, 6, 1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 2, 3, 3, 4, 3, 4, 4,
    5, 3, 4, 4, 5, 4, 5, 5, 6, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 3, 4, 4, 5, 4, 5, 5,
    6, 4, 5, 5, 6, 5, 6, 6, 7, 1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 2, 3, 3, 4, 3, 4, 4,
    5, 3, 4, 4, 5, 4, 5, 5, 6, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 3, 4, 4, 5, 4, 5, 5,
    6, 4, 5, 5, 6, 5, 6, 6, 7, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 3, 4, 4, 5, 4, 5, 5,
    6, 4, 5, 5, 6, 5, 6, 6, 7, 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7, 4, 5, 5, 6, 5, 6, 6,
    7, 5, 6, 6, 7, 6, 7, 7, 8};

  final static int maskByte=255;

  public static int countBits(int bits) {
    return countLUT[bits&maskByte]+countLUT[(bits>>>8)&maskByte]+countLUT[(bits>>>16)&maskByte];
  }

}

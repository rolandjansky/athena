package atlantis.geometry;

import atlantis.parameters.APar;
import java.util.*;

/**
 * Used to store all detectors which are not part of the muon system
 * these may be of different types.
 */

public final class AGeneralDetectors extends ADetectors {

  public AGeneralDetectors(List detectors) {
   super(filterGeneralDetectors(detectors));
  }

  private static ADetector[] filterGeneralDetectors(List detectors) {

    List tempDetectors=new ArrayList(detectors.size());
    Iterator it=detectors.iterator();

    while(it.hasNext()) {
      Object o=it.next();
      if((o instanceof ADetector)&&!(o instanceof ABoxDetector)&&!(o instanceof ATrapezoidDetector)) tempDetectors.add(o);
    }
    return (ADetector[])tempDetectors.toArray(new ADetector[tempDetectors.size()]);
  }

  private void makeDrawListYX() {
    int mode=parameterStore.get("YX", "Mode").getI();
    if(mode==0) {
      int num=0;
      for(int i=0; i<numData; ++i)
        if(det[i].getName().indexOf("FCAL")<0)
          listdl[num++]=i;
      numDraw=num;
    } 
    else if(mode>8) 
    {
      int num=0;
      for(int i=0; i<numData; ++i)
        if((mode==9 && det[i].getName().equals("FCAL EM"))
            ||(mode==10 && det[i].getName().equals("FCAL HAD 1"))
            ||(mode==11 && det[i].getName().equals("FCAL HAD 2"))
            ||(mode==12 && (det[i].getName().equals("LAr Endcap Presampler"))||det[i].getName().equals("LAr_EC_Presampler"))
            ||(mode>=13 && mode<= 15 && (det[i].getName().equals("LAr Outer Endcap") || det[i].getName().equals("LAr Inner Endcap")))
            ||(mode>=16 && mode<= 19 && det[i].getName().equals("HEC"))
            ||(parameterStore.get("YX", "DrawFCAL").getStatus() && mode==17 && det[i].getName().equals("FCAL EM"))
            ||(parameterStore.get("YX", "DrawFCAL").getStatus() && mode==18 && det[i].getName().equals("FCAL HAD 1"))
            ||(parameterStore.get("YX", "DrawFCAL").getStatus() && mode==19 && det[i].getName().equals("FCAL HAD 2"))
            ||(mode==20 && (det[i].getName().indexOf("LAr")>=0 || det[i].getName().indexOf("Endcap")>=0 || det[i].getName().indexOf("EC")>=0))
            ||(mode==21 && det[i].getName().equals("HEC"))
            ||(mode==22)&& det[i].getName().equals("Minimum Bias Trigger Scintillators"))
          listdl[num++]=i;
      numDraw=num;
    } else
      numDraw=0;
  }

  protected void makeDrawList(String projection) {
    if (projection.equals("YX") || projection.equals("FR"))
      makeDrawListYX();
    else {
      if(parameterStore.get(projection, "Mode").getI()==0)
        constructDefaultDrawList();
      else
        numDraw=0;
    }
  }

}

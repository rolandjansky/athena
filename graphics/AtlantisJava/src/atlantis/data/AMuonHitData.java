package atlantis.data;

import atlantis.event.*;
import atlantis.globals.AGlobals;
import atlantis.utils.*;
import atlantis.parameters.*;

public abstract class AMuonHitData extends AHitData {

  protected float[] rho;
  protected float[] phi;
  protected float[] x;
  protected float[] y;
  protected float[] z;
  protected int[] sector;
  protected int[] station;
  protected boolean[] measuresPhi;

  public static final int DOES_NOT_MEASURE_PHI=0;
  public static final int MEASURES_PHI=1;
  
  // remove when go to compact id ( if compact id was in a sensible form)
  String[] identifier;

  AMuonHitData(AHashMap p, AEvent e) {
    super(p,e);
    x=p.getFloatArray("x");
    y=p.getFloatArray("y");
    z=p.getFloatArray("z");
    rho=new float[numData];
    phi=new float[numData];
    for(int i=0; i<numData; ++i) {
      rho[i]=(float)Math.sqrt(x[i]*x[i]+y[i]*y[i]);
      phi[i]=(float)Math.atan2(y[i], x[i]);
      if(phi[i]<0.) phi[i]+=AMath.TWO_PI;
    }
    identifier=p.getStringArray("identifier");
    station=new int[numData];
    sector=new int[numData];
    measuresPhi=new boolean[numData];
    for(int i=0; i<numData; ++i) {
      // should be moved to AHitData
      sub[i]=getSub(i);
      sector[i]=getSector(i);
      station[i]=getStation(i);
      measuresPhi[i]=getMeasuresPhi(i);
    }
  }

  /**
   * Get the name used for associating this datatype/collection. For the 
   * muon hits no storegate key is used.
   * @return 
   */
  public String getFullName() {
    return getName();
  }

  protected int internalColor()
  {
      int numColorTypes = super.internalColor();
      int colorFunction = parameterStore.get(PARAMETER_GROUP, "ColorFunction").getI();

      if(colorFunction == numColorTypes + 1) // colour by Track Segment
      {
          colorBy(getSegments());
      }
      return numColorTypes + 1;

  } // internalColor() ----------------------------------------------------


  abstract protected int getStation(int index);

  abstract public int getSector(int index);

  abstract protected int getSub(int index);

  abstract protected boolean getMeasuresPhi(int index);

  protected String getSegments() {
      AEnumeratorParameter key = (AEnumeratorParameter) parameterStore.get("MuonSegment", "MuonSegmentCollections");

      return "MuonSegment" + key.getCurrentText();
  }

  public int getRZSign(int index) {
    double phiMid=Math.toRadians(parameterStore.get("RZ", "Phi").getD());
    double phiDiff=Math.abs(phi[index]-phiMid);

    int sign=-1;
    if(phiDiff<Math.PI/2.||phiDiff>3*Math.PI/2.)
      sign=1;
    return sign;
  }

  protected String getReconstructedTracks() {
    AData tracks = event.getTrackData("MuonTrack");
    if (tracks == null) {
        return null;
    } else {
        return tracks.getName() + tracks.getStoreGateKey();
    }
  }

  // get the distance of closest approach of the inifinite line representing the
  // drift tube axis to Z axis
  // Normal this is rho of the hit, but need not be for non standard modules
  // (e.g. near feet)
   static double getSectorRho(int sector, double rho, double phi) {
    return rho*Math.abs(Math.cos(phi-sector*AMath.PI_BY_8));
  }

  protected void applyCuts() {
    cutIndex();
    cutSimulatedTracks();
    cutReconstructedTracks();
    cutSegments();
  }

  // keep only muon hits in the input muon sectors
  protected void cutMuonSector(int[] sector) {
    AParameter p=parameterStore.get("XZ", "Phi");

    // display muon data when phi is in the middle of a sector
    if(p.getD() % 22.5 < 1e-2) {
      int sect=(int)Math.round((p.getD() % 360.) / 22.5);

      int num=0;

      for(int i=0; i<numDraw; i++) {
        int s=sector[listdl[i]];

        if(s==sect||s==sect-8||s==sect+8)
          listdl[num++]=listdl[i];
      }
      numDraw=num;
    } else {
      numDraw=0;
    }
  }

  public String getHitInfo(int index)
  {
  	int simpleOutput = AGlobals.instance().getSimpleOutput();
	if(simpleOutput>0) return getNameScreenName()+" index: " + index+"\n"+
    AMath.RHO+" = "+String.format("%.3f",rho[index])+"\n "+
    "z = " + String.format("%.3f",z[index]) + "cm\n"+
    AMath.PHI+" = "+String.format("%.3f",Math.toDegrees(phi[index]))+AMath.DEGREES;
      
    String temp = getNameScreenName() + " (id: " + identifier[index] + " index: " + index + ")" +
                  "\n " + AMath.RHO + " = " + String.format("%.3f",rho[index]) + " cm" +
                  "\n " + AMath.PHI + " = " + String.format("%.3f",Math.toDegrees(phi[index])) +
                  AMath.DEGREES + "  (" + String.format("%.3f",phi[index]) + " rad)" +
                  "\n z = " + String.format("%.3f",z[index]) + " cm";

    if(sector[index]!=-1)
    {
        temp += "\n sector = " + sector[index];
    }

    return temp;
  }

  protected void setType() { //What is this doing??
    int[][] recon=event.getAssociationManager().get(getName(),getReconstructedTracks());
    for(int i=0; i<numData; i++) {
      if( (recon!=null&&recon[i]!=null))
        type[i]=1;
      else
        type[i]=0;
    }
  }

  /**
   * cut hits if connected/unconnected to reconstructed tracks
   */
  protected void cutReconstructedTracks()
  {
      int cutOption = parameterStore.get("CutsMuon", "HitsByRTr").getI();
      cutByAssociationTo(getReconstructedTracks(), getName(), cutOption);
  }
  
  /**
   * cut hits if connected/unconnected to reconstructed segments
   */
  protected void cutSegments()
  {
      int cutOption = parameterStore.get("CutsMuon", "HitsBySegment").getI();
      cutByAssociationTo(getSegments(), getName(), cutOption);
  }
}

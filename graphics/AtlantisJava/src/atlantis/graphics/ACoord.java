package atlantis.graphics;

import atlantis.parameters.APar;
import atlantis.utils.*;

/**
 * Used to store a collection of User or display 2D coordinates of hits or tracks
 * together with their indices in the corresponding AData
 * (used for picking, colors etc.). N.B. the storage model is different for
 * SYMBOLS/LINES(with two end points) than for POLYLINES/POLYGONS for
 * efficiency reasons. Transformations between user/display coordinaates are based
 * on ACoords.
 */
public class ACoord {
    
  private static ALogger logger = ALogger.getLogger(ACoord.class);
    
  public double[][][] hv;
  public int[] index;
  protected int type=POLYGONS;
  public ADrawable source;

  final static public int SYMBOLS=1;
  final static public int LINES=2;
  final static public int POLYLINES=3;
  final static public int POLYGONS=4;
  final static public int DOTTED_POLYGONS=5;  // for drawing missing ET in V-Plot
  final static public int SMOOTH_POLYLINES=6;
  final static public int TRANSPARENT_POLYGONS=7;

  final static public ACoord NO_DATA=new ACoord(new double[2][0][0], new int[0]);
  final static public ACoord NO_POLYGONS=new ACoord(new double[2][0][0], new int[0]);
  final static public ACoord NO_POLYLINES=new ACoord(new double[2][0][0], new int[0]);
  final static public ACoord[] NO_HISTOGRAMS=new ACoord[0];

  protected static APar parameterStore = APar.instance();
  
  public ACoord(double h, double v, int index) {
    this(h, v);
    this.index=new int[] {index};
  }

  public ACoord(double h, double v) {
    this.hv=new double[2][1][1];
    this.hv[0][0][0]=h;
    this.hv[1][0][0]=v;
    this.index=null;
    this.type=SYMBOLS;
  }

  public ACoord(double[] h, double[] v) {
    this.hv=new double[2][1][];
    this.hv[0][0]=h;
    this.hv[1][0]=v;
    this.index=new int[] {0};
  }

  private ACoord(double[][] hv, int[] index) {
    if(hv.length!=2) throw new IllegalArgumentException("hv.length="+hv.length);
    this.hv = new double[2][1][0];
    this.hv[0][0]=hv[0];
    this.hv[1][0]=hv[1];
    this.index=index;
  }

  private ACoord(double[][] hv, int[] index, ADrawable source) {
    this(hv, index);
    this.source=source;
  }

  public ACoord(double[][] hv) {
      if(hv.length!=2) throw new IllegalArgumentException("hv.length="+hv.length);
      this.hv = new double[2][1][];
      this.hv[0][0]=hv[0];
      this.hv[1][0]=hv[1];
      this.index=null;
  }

  public ACoord(double[][][] hv, int[] index) {
    if(hv.length!=2) throw new IllegalArgumentException("hv.length="+hv.length);
    this.hv=hv;
    this.index=index;
  }

  public ACoord(double[][][] hv, int[] index, ADrawable source) {
    this(hv, index);
    this.source=source;
  }

  public ACoord(double[][][] hv) {
    if(hv.length!=2) throw new IllegalArgumentException("hv.length="+hv.length);
    this.hv=hv;
    this.index=null;
  }

  private ACoord(ACoord a) {
    this.hv=new double[a.hv.length][][];
    for(int i=0; i<a.hv.length; i++) {
      this.hv[i]=new double[a.hv[i].length][];
      for(int j=0; j<a.hv[i].length; j++) {
        this.hv[i][j]=new double[a.hv[i][j].length];
        for(int k=0; k<a.hv[i][j].length; k++)
          this.hv[i][j][k]=a.hv[i][j][k];
      }
    }
    this.index=new int[a.index.length];
    for(int i=0; i<a.index.length; i++)
      this.index[i]=a.index[i];
    this.type=a.type;
    this.source=a.source;
  }

  public ACoord(double[][] hv, int[] index, ADrawable source, int type) {
    this(hv, index, source);
    this.type=type;
  }

  public ACoord(double[][][] hv, int[] index, ADrawable source, int type) {
    this(hv, index, source);
    this.type=type;
  }

  public ACoord(double[] h, double[] v, int[] index, ADrawable source) {
    if(h.length!=v.length)
      throw new IllegalArgumentException("h.length "+h.length+" !=v.length "+v.length);
    if(h.length!=index.length)
      throw new IllegalArgumentException("h.length "+h.length+" !=index.length "+index.length);
    this.hv=new double[2][1][];
    this.hv[0][0]=h;
    this.hv[1][0]=v;
    this.index=index;
    this.type=SYMBOLS;
    this.source=source;
  }

  public void print() {
    for(int j=0; j<hv[0].length; j++) {
      logger.debug(" index = "+index[j]);
      for(int i=0; i<2; i++) {
        if(i==0)
          logger.debug("h ");
        else
          logger.debug("v ");
        for(int k=0; k<hv[0][j].length; k++)
          logger.debug((float)hv[i][j][k]+"\t");
      }
    }
  }

  // careful this does not make an independent copy
  public ACoord(ACoord a, ACoord b) {
    this.hv=new double[2][a.hv[0].length+b.hv[0].length][];
    this.index=new int[a.hv[0].length+b.hv[0].length];
    this.type=a.type;
    this.source=a.source;

    for(int i=0; i<2; i++) {
      int num=0;

      for(int j=0; j<a.hv[i].length; j++) {
        this.hv[i][num]=a.hv[i][j];
        this.index[num]=a.index[j];
        num++;
      }
      for(int j=0; j<b.hv[i].length; j++) {
        this.hv[i][num]=b.hv[i][j];
        this.index[num]=b.index[j];
        num++;
      }
    }
  }

  public ACoord convertYXToFR() {
    if(type==SYMBOLS||type==LINES) {
      for(int j=0; j<hv[0][0].length; j++) {
        double phiLast=0.;

        for(int i=0; i<hv[0].length; i++) {
          double xx=hv[0][i][j];
          double yy=hv[1][i][j];
          double rho=Math.sqrt(xx*xx+yy*yy);
          double phi=Math.toDegrees(Math.atan2(yy, xx));

          if(phi<0.)
            phi+=360.;
          if(i>0) {
            if(phi-phiLast>180.) phi-=360.;
            if(phi-phiLast<-180.) phi+=360.;
          }
          hv[0][i][j]=rho;
          hv[1][i][j]=phi;
          phiLast=phi;
        }
      }

    } else {
      for(int i=0; i<hv[0].length; i++) {
        double phiLast=0.;

        for(int j=0; j<hv[0][i].length; j++) {
          double xx=hv[0][i][j];
          double yy=hv[1][i][j];
          double rho=Math.sqrt(xx*xx+yy*yy);
          double phi=Math.toDegrees(Math.atan2(yy, xx));

          if(phi<0.)
            phi+=360.;
          if(j>0) {
            if(phi-phiLast>180.) phi-=360.;
            if(phi-phiLast<-180.) phi+=360.;
          }
          hv[0][i][j]=rho;
          hv[1][i][j]=phi;
          phiLast=phi;
        }
      }
    }
    return this;
  }

  public ACoord includePhiWrapAround(String projectionName) {
    // assumes phi is always the v coordinaate
    int numFrames=parameterStore.get(projectionName, "FramesCount").getI();
    int initialFrame=parameterStore.get(projectionName, "InitialFrame").getI();

    if(type==SYMBOLS||type==LINES) {
      int numPoints=hv[1].length;
      int numInitialData=hv[1][0].length;

      double[][][] hvtemp=new double[2][numPoints][numFrames*numInitialData];
      int[] indextemp=new int[numFrames*numInitialData];

      int num=0;

      for(int f=initialFrame; f<initialFrame+numFrames; f++) {
        for(int i=0; i<numInitialData; i++) {
          if(index!=null) indextemp[num]=index[i];
          for(int j=0; j<numPoints; ++j) {
            hvtemp[0][j][num]=this.hv[0][j][i];
            hvtemp[1][j][num]=this.hv[1][j][i]+f*360.;
          }
          num++;
        }
      }
      hv=hvtemp;
      if (index!=null) index=indextemp;
    } else {
      int numInitialData=hv[1].length;
      double[][][] hvtemp=new double[2][numFrames*numInitialData][];
      int[] indextemp=new int[numFrames*numInitialData];

      int num=0;

      for(int f=initialFrame; f<initialFrame+numFrames; f++) {
        for(int i=0; i<numInitialData; i++) {
          int numPoints=hv[1][i].length;

          hvtemp[0][num]=new double[numPoints];
          hvtemp[1][num]=new double[numPoints];
          for(int j=0; j<numPoints; j++) {
            hvtemp[0][num][j]=this.hv[0][i][j];
            hvtemp[1][num][j]=this.hv[1][i][j]+f*360.;
          }
          if (index!=null) indextemp[num]=index[i];
          num++;
        }
      }
      hv=hvtemp;
      if (index!=null) index=indextemp;
    }
    return this;
  }

  /**
   *  used in ATLAS context for the TRT
   *  Force data to be drawn as symbols if
   *   forceSymbols=true
   *  or
   *   all lines are smaller than minSize
   */
  public ACoord toSymbols(boolean forceSymbols, int minSize) {
    if(type==LINES) {
      double dmax2=0.;

      for(int i=0; i<hv[0][0].length; i++) {
        double dh=(hv[0][0][i]-hv[0][1][i]);
        double dv=(hv[1][0][i]-hv[1][1][i]);
        double d=dh*dh+dv*dv;

        if(d>dmax2) dmax2=d;
      }
      if(Math.sqrt(dmax2)<minSize||forceSymbols) {
        type=SYMBOLS;
        for(int i=0; i<hv[0][0].length; i++) {
          hv[0][0][i]=(hv[0][0][i]+hv[0][1][i])/2.;
          hv[1][0][i]=(hv[1][0][i]+hv[1][1][i])/2.;
        }
      }
    } else if(type==POLYLINES||type==POLYGONS||type==TRANSPARENT_POLYGONS) {
      double dmax2=0.;

      for(int j=0; j<hv[0].length; j++) {
        for(int i=0; i<hv[0][j].length; i++) {
          double dh=(hv[0][j][0]-hv[0][j][i]);
          double dv=(hv[1][j][0]-hv[1][j][i]);
          double d=dh*dh+dv*dv;

          if(d>dmax2) dmax2=d;
        }
      }

      if(Math.sqrt(dmax2)<minSize||forceSymbols) {
        type=SYMBOLS;
        double[][][] hvnew=new double[2][1][hv[0].length];

        for(int j=0; j<hv[0].length; j++) {
          double h=0.;
          double v=0.;

          for(int i=0; i<hv[0][j].length; i++) {
            h+=hv[0][j][i];
            v+=hv[1][j][i];
          }
          h/=hv[0][j].length;
          v/=hv[0][j].length;
          hvnew[0][0][j]=h;
          hvnew[1][0][j]=v;
        }
        hv=hvnew;
      }
    }
    return this;
  }

  public ACoord mirrorH () {
    ACoord coord = new ACoord(this);
    for(int i=0; i<coord.hv[0].length; i++) {
      for(int j=0; j<coord.hv[0][i].length; j++) {
        coord.hv[0][i][j] *= -1;
      }
    }
    return coord;
  }
  
  public ACoord mirrorV () {
    ACoord coord = new ACoord(this);
    for(int i=0; i<coord.hv[1].length; i++) {
      for(int j=0; j<coord.hv[1][i].length; j++) {
        coord.hv[1][i][j] *= -1;
      }
    }
    return coord;
  }
}


package atlantis.data;

import atlantis.event.*;
import atlantis.graphics.ACoord;
import atlantis.parameters.AParameterUtilities;
import atlantis.projection.*;
import atlantis.utils.*;


public abstract class A3DPointData extends AHitData {

  protected float[] rho;
  protected float[] phi;
  protected float[] z;
  protected float[] x;
  protected float[] y;


  A3DPointData(AHashMap p, AEvent e) {
    super(p,e);
    if(p.get("x")!=null) {
      x=p.getFloatArray("x");
      y=p.getFloatArray("y");
      z=p.getFloatArray("z");
      rho=new float[numData];
      phi=new float[numData];

      // this is needed for the moment so that S3D sub and layer can be
      // calculated from rph,phi,z rho phi from x y
      for(int i=0; i<rho.length; ++i) {
        double dx=x[i];
        double dy=y[i];

        rho[i]=(float)(Math.sqrt(dx*dx+dy*dy));
        phi[i]=(float)(Math.atan2(dy, dx));
        if(phi[i]<0.) phi[i]+=AMath.TWO_PI;
      }
    } else {
      rho=p.getFloatArray("rho");
      phi=p.getFloatArray("phi");
      z=p.getFloatArray("z");
      x=new float[numData];
      y=new float[numData];
      calculateXY(rho, phi, x, y);
    }
  }


  public float[] getRho()
  {
      return rho;
  }


  public float[] getPhi()
  {
      return phi;
  }


  public float[] getX()
  {
      return x;
  }


  public float[] getY()
  {
      return y;
  }


  public float[] getZ()
  {
      return z;
  }


  protected void calculateRhoPhi()
  {
      calculateRhoPhi(x, y, rho, phi);
  }

  protected ACoord getYXUser() {
    makeDrawList();
    double[] h=new double[numDraw];
    double[] v=new double[numDraw];
    int[] index=new int[numDraw];

    for(int i=0; i<numDraw; i++) {
      int list=listdl[i];
      h[i]=x[list];
      v[i]=y[list];
      index[i]=list;
    }
    return new ACoord(h, v, index, this);
  }

  protected ACoord getRZUser() {
    makeDrawList();
    double[] h=new double[numDraw];
    double[] v=new double[numDraw];
    int[] index=new int[numDraw];

    double phiMid=Math.toRadians(parameterStore.get("RZ", "Phi").getD());
    if(phiMid>AMath.TWO_PI) phiMid-=AMath.TWO_PI;

    for(int i=0; i<numDraw; i++) {
      int list=listdl[i];
      h[i]=z[list];
      double phiDiff=Math.abs(phi[list]-phiMid);
      if(phiDiff<Math.PI/2.||phiDiff>3*Math.PI/2.)
      {
          v[i] = rho[list];
      }
      else
      {
          v[i] = -rho[list];
      }
      index[i]=list;

    }
    return new ACoord(h, v, index, this);
  }

  protected ACoord getXZUser() {
    makeDrawList();
    double[] h=new double[numDraw];
    double[] v=new double[numDraw];
    int[] index=new int[numDraw];

    double phi0=Math.toRadians(AProjectionXZ.getPhi());
    double cosPhi0=Math.cos(phi0);
    double sinPhi0=Math.sin(phi0);

    for(int i=0; i<numDraw; i++) {
      int list=listdl[i];
      h[i]=z[list];
      v[i]=x[list]*cosPhi0+y[list]*sinPhi0;
      index[i]=list;
    }
    return new ACoord(h, v, index, this);
  }

  protected ACoord getYZUser() {
    makeDrawList();
    double[] h=new double[numDraw];
    double[] v=new double[numDraw];
    int[] index=new int[numDraw];

    double phi0=Math.toRadians(AProjectionXZ.getPhi());
    double cosPhi0=Math.cos(phi0);
    double sinPhi0=Math.sin(phi0);

    for(int i=0; i<numDraw; i++) {
      int list=listdl[i];
      h[i]=z[list];
      v[i]=y[list]*cosPhi0-x[list]*sinPhi0;
      index[i]=list;
    }
    return new ACoord(h, v, index, this);
  }

  protected ACoord get3DUser() {
    makeDrawList();
    double[][] hvo=AProjection3D.getRotated(numDraw, listdl, x, y, z);
    double[] h=hvo[0];
    double[] v=hvo[1];
    int[] index=new int[numDraw];
    for(int i=0; i<numDraw; i++) {
      int list=listdl[i];
      index[i]=list;
    }
    return new ACoord(h, v, index, this);
  }

// does not fit in normal scheme
  public double[][] get3DPoints() {
    makeDrawList();
    double[][] xyz =new double[numDraw][3];
    for(int i=0; i<numDraw; i++) {
      int list=listdl[i];
      xyz[i][0]=x[list];
      xyz[i][1]=y[list];
      xyz[i][2]=z[list];
    }
    return xyz;
  }

// does not fit in normal scheme
  public int[] get3DPointsIndex() {
    makeDrawList();
    int[] index=new int[numDraw];
    for(int i=0; i<numDraw; i++)
      index[i]=listdl[i];
    return index;
  }

  protected ACoord getFZUser() {
    makeDrawList();
    double[] h=new double[numDraw];
    double[] v=new double[numDraw];
    int[] index=new int[numDraw];

    for(int i=0; i<numDraw; i++) {
      int list=listdl[i];
      h[i]=z[list];
      v[i]=Math.toDegrees(phi[list]);
      index[i]=list;
    }
    return new ACoord(h, v, index, this).includePhiWrapAround("FZ");
  }

  protected ACoord getFRUser() {
    makeDrawList();
    double[] h=new double[numDraw];
    double[] v=new double[numDraw];
    int[] index=new int[numDraw];

    for(int i=0; i<numDraw; i++) {
      int list=listdl[i];
      h[i]=rho[list];
      v[i]=Math.toDegrees(phi[list]);
      index[i]=list;
    }
    return new ACoord(h, v, index, this).includePhiWrapAround("FR");
  }

  protected ACoord getVPUser() {
    makeDrawList();
    // each point is drawn twice
    int numTotal=2*numDraw;
    double[] h=new double[numTotal];
    double[] v=new double[numTotal];
    int[] index=new int[numTotal];

    double[] sign=new double[] {-1., 1.};
    int num=0;

    for(int i=0; i<numDraw; i++) {
      int list=listdl[i];
      double eta=AParameterUtilities.eta(z[list], rho[list]);
      double delEta=AProjectionVP.getDeltaEta(rho[list], z[list]);
      for(int j=0; j<2; j++) {
        h[num]=eta+sign[j]*delEta;
        v[num]=AParameterUtilities.phi(x[list], y[list]);
        index[num]=list;
        num++;
      }
    }
    return new ACoord(h, v, index, this).includePhiWrapAround("VP");
  }

}

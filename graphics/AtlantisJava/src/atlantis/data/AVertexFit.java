package atlantis.data;

import atlantis.parameters.APar;
import atlantis.utils.ALogger;

import Jama.*;


/**
 * This class provide a java implementation of the ALEPH vertex fitting
 * routine YTOPOL. At the moment the only functionality is the fitting
 * of ATLAS helices to a common vertex. A reasonable starting vertex for the
 * fit must be given. For B decays the primary vertex should be good enough.
 * Future improvements in functionality could include estimation of a
 * starting vertex and including in the fitting also already calculated
 * vertices and charged and neutral particles
 */

public class AVertexFit {
  private static ALogger logger = ALogger.getLogger(AVertexFit.class);
  private static APar parameterStore = APar.instance();
  
  private static final int MXITR=8;
  private static final double CHISC=1.E-2;
  private static final double CHISR=0.01;
  private static final double PARCR=0.01;
  private static final boolean DEBUG=false;

  private AVertexFit() {}

  public static AVertex fitVertex(AVertex estimatedVertex, AHelix[] helix) {
    Matrix[] measuredHelix=new Matrix[helix.length];
    Matrix[] covHelix=new Matrix[helix.length];
    Matrix[] errHelix=new Matrix[helix.length];

    for(int k=0; k<helix.length; ++k) {
      double[] par=helix[k].getPar();
      double[][] cov=helix[k].getCovariance();
      double signD0=-1.;
      double signPt=1.;

      par[0]*=signD0;
      par[4]*=signPt;

      for(int i=0; i<5; ++i) {
        cov[0][i]*=signD0;
        cov[i][0]*=signD0;
        cov[4][i]*=signPt;
        cov[i][4]*=signPt;
      }

      measuredHelix[k]=new Matrix(par, par.length);
      covHelix[k]=new Matrix(cov);
      errHelix[k]=covHelix[k].inverse();

      // AOutput.logln( "measuredHelix "+k+" "+measuredHelix[k]);
      // AOutput.logln( measuredHelix[k].toString());
      // AOutput.logln( covHelix[k].toString());

    }

    int numFreeParam=3+3*helix.length;
    Matrix fitted=new Matrix(numFreeParam, 1);
    double[] estVertex=estimatedVertex.getPosition();
    int ipar;

    for(ipar=0; ipar<3; ++ipar)
      fitted.set(ipar, 0, estVertex[ipar]);
    ipar=3;
    for(int k=0; k<helix.length; ++k) {
      fitted.set(ipar++, 0, parameterStore.get("Event", "Curvature").getD()/measuredHelix[k].get(4, 0)-measuredHelix[k].get(0, 0));

      fitted.set(ipar++, 0, measuredHelix[k].get(3, 0));
      fitted.set(ipar++, 0, measuredHelix[k].get(2, 0));
    }

    logger.debug("fittedHelix "+0+" "+new Transformation(fitted, 0).helixFromFitted());
    logger.debug("fittedHelix "+1+" "+new Transformation(fitted, 1).helixFromFitted());

    // AOutput.logln( fitted.toString());

    Matrix g=new Matrix(numFreeParam, 1);
    Matrix gg=new Matrix(numFreeParam, numFreeParam);
    int iteration=0;
    double chiso=0.;
    double chisq=0.;
    boolean converged;

    FITTINGLOOP:
    do {
      g.timesEquals(0.);
      gg.timesEquals(0.);
      for(int k=0; k<helix.length; ++k) {
        Transformation t=new Transformation(fitted, k);

        Matrix fittedHelix=t.helixFromFitted();

        logger.debug("fittedHelix "+k+" "+fittedHelix);
        Matrix dMdF=t.getHelixdMdF();
        Matrix delFM=fittedHelix.minus(measuredHelix[k]);
        // helices are independent of each other so can
        // use temporary matrices to avoid large matrix multiplications
        Matrix gtemp=(errHelix[k].times(dMdF)).transpose().times(delFM);
        Matrix ggtemp=dMdF.transpose().times(errHelix[k].times(dMdF));

        // now put these into the large matrices
        for(int i=0; i<3; ++i) {
          int ix=3+3*k;

          g.set(i, 0, g.get(i, 0)+gtemp.get(i, 0));
          g.set(ix+i, 0, g.get(ix+i, 0)+gtemp.get(3+i, 0));
          for(int j=0; j<3; ++j) {
            gg.set(i, j, gg.get(i, j)+ggtemp.get(i, j));
            gg.set(ix+i, j, gg.get(ix+i, j)+ggtemp.get(3+i, j));
            gg.set(i, ix+j, gg.get(i, ix+j)+ggtemp.get(i, 3+j));
            gg.set(ix+i, ix+j, gg.get(ix+i, ix+j)+ggtemp.get(3+i, 3+j));
          }
        }
        if(iteration==0) {
          double chih=delFM.transpose().times(errHelix[k].times(delFM)).get(0, 0);

          chiso+=Math.min(chih, 1.0e+10);
          // AOutput.logln( "initial chi2"+chih);
        }
      }

      // AOutput.logln( gg.toString());

      gg=gg.inverse();
      Matrix delFitted=gg.times(g);

      logger.debug("DELFITTED "+iteration+" "+delFitted.toString());

      fitted.minusEquals(delFitted);

      int jter=0;
      int kter=0;

      while(true) {
        if(kter>10) break FITTINGLOOP;
        if(jter>100) break FITTINGLOOP;

        double chish=0.;

        chisq=0.;
        for(int k=0; k<helix.length; ++k) {
          Transformation t=new Transformation(fitted, k);
          Matrix fittedHelix=t.helixFromFitted();
          Matrix delFM=fittedHelix.minus(measuredHelix[k]);
          double chih=delFM.transpose().times(errHelix[k].times(delFM)).get(0, 0);

          chish+=Math.min(chih, 1.0e+10);
        }
        chisq+=chish;

        double chis1=0.;

        if((chisq>chiso+0.0001&&jter==0)||chisq>1.1*chiso) {
          delFitted.timesEquals(0.5);
          fitted.plusEquals(delFitted);
          jter++;
          chis1=chisq;
          continue;
        } else {
          if(jter>0) {
            // estimate best parameters
            double chtrm=chis1+chiso-2.*chisq;
            double fx;

            if(chtrm>0.) {
              // concave chisq dependance
              fx=(chis1-chiso)/(2.*(chis1+chiso-2.*chisq));
              fx=Math.max(fx, -2.);
              fx=Math.min(fx, 2.);
            } else {
              // convex chisq dependance
              fx=-2.;
            }
            delFitted.timesEquals(fx);

            jter=0;
            kter++;
            continue;
          }
        }
        break;
      }

      double dchi2=chiso-chisq;

      chiso=chisq;
      converged=true;
      // check for change in chisq
      if(dchi2>CHISC&&dchi2>CHISR*chisq) converged=false;
      // check for change in parameters
      for(int i=0; i<numFreeParam; i++)
        if(delFitted.get(i, 0)*delFitted.get(i, 0)>PARCR*gg.get(i, i))
          converged=false;
    } while((!converged)&&iteration++<MXITR);

    double[] fittedVertex=new double[3];
    double[][] fittedVertexCov=new double[3][];

    for(int i=0; i<3; i++) {
      fittedVertex[i]=fitted.get(i, 0);
      fittedVertexCov[i]=new double[3];
      for(int j=0; j<3; j++)
        fittedVertexCov[i][j]=gg.get(i, j);
    }

    return new AVertex(fittedVertex, fittedVertexCov, chisq);
  }

  private static class Transformation {
	  double rho, tau, phi0, d0, z0, sinPhi, cosPhi, sinDPhi, cosDPhi, eta, s, r, dPhi;

	  Transformation(Matrix fitted, int iHelix) {
		  int index=3+3*iHelix;

		  eta=fitted.get(index++, 0);
		  tau=fitted.get(index++, 0);
		  phi0=fitted.get(index++, 0);
		  double sinPhi0=Math.sin(phi0);
		  double cosPhi0=Math.cos(phi0);
		  double xC=-eta*sinPhi0;
		  double yC=eta*cosPhi0;

		  // if(DEBUG) AOutput.logln(" Afit xCyC"+iHelix+" "+xC+" "+yC+" "+eta);

		  double xVxC=fitted.get(0, 0)-xC;
		  double yVyC=fitted.get(1, 0)-yC;

		  r=0.;
		  if(eta>0.)
			  r=Math.sqrt(xVxC*xVxC+yVyC*yVyC);
		  else
			  r=-Math.sqrt(xVxC*xVxC+yVyC*yVyC);

		  d0=r-eta;
		  sinPhi=xVxC/r;
		  cosPhi=-yVyC/r;
		  double phi=Math.atan2(sinPhi, cosPhi);

		  if(phi-phi0<-Math.PI) phi+=2.*Math.PI;
		  dPhi=phi-phi0;
		  s=dPhi*r;
		  z0=fitted.get(2, 0)-s*tau;
		  sinDPhi=Math.sin(dPhi);
		  cosDPhi=Math.cos(dPhi);
		  rho=1./r;
	  }

	  Matrix getHelixdMdF() {
		  Matrix dMdF=new Matrix(5, 6);

		  double curvature = parameterStore.get("Event", "Curvature").getD();
		  dMdF.set(4, 0, -curvature*rho*rho*sinPhi);
		  dMdF.set(4, 1, curvature*rho*rho*cosPhi);
		  dMdF.set(4, 3, -curvature*rho*rho*cosDPhi);
		  dMdF.set(4, 5, -curvature*rho*rho*eta*sinDPhi);
		  dMdF.set(3, 4, 1.);
		  dMdF.set(2, 5, 1.);
		  dMdF.set(0, 0, sinPhi);
		  dMdF.set(0, 1, -cosPhi);
		  dMdF.set(0, 3, cosDPhi-1.);
		  dMdF.set(0, 5, eta*sinDPhi);
		  dMdF.set(1, 0, -tau*(sinPhi*dPhi+cosPhi));
		  dMdF.set(1, 1, tau*(cosPhi*dPhi-sinPhi));
		  dMdF.set(1, 2, 1.);
		  dMdF.set(1, 3, -tau*(dPhi*cosDPhi-sinDPhi));
		  dMdF.set(1, 4, -s);
		  dMdF.set(1, 5, -tau*(eta*(dPhi*sinDPhi+cosDPhi)-r));
		  return dMdF;
	  }

	  Matrix helixFromFitted() {
		  Matrix helix=new Matrix(5, 1);

		  helix.set(4, 0, rho*parameterStore.get("Event", "Curvature").getD());
		  helix.set(3, 0, tau);
		  helix.set(2, 0, phi0);
		  helix.set(0, 0, d0);
		  helix.set(1, 0, z0);
		  return helix;
	  }

  }
}


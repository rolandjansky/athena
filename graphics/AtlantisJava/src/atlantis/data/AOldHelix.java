package atlantis.data;

import atlantis.globals.AGlobals;
import atlantis.parameters.APar;
import atlantis.parameters.AParameter;
import atlantis.utils.AMath;
import atlantis.utils.A3Vector;
import atlantis.utils.ALogger;

import static java.lang.Math.PI;

/**
 * This is a convenience class to group together the six helix parameters.
 */
public class AOldHelix {

  private final static ALogger logger = ALogger.getLogger(AOldHelix.class);

  private static final int SIZE=5;
  private static final boolean DEBUG = false;
  private static final APar parameterStore = APar.instance();
  // C - constant depending on the magnetic field, constant proportional
  // to curvature in the magnetic field
  private static final double C;
  static {
	  AParameter curvatureParameter = parameterStore.get("Event", "Curvature");
	  if (curvatureParameter!=null) {
		  C = curvatureParameter.getD();
	  }
	  else {
		  C = 100/0.6;;
		  logger.error("Curvature parameter not found, defaulting to "+C);
	  }
  }
  
  // These fields are public but final so cannot be altered.
  /** Distance between circle and (0,0) at point of closest approach (PCA). */
  public final double d0;
  /** Z of circle at PCA. */
  public final double z0;
  /** phi0 of track at PCA [degrees] */
  public final double phi0;
  /** Dip of track = Pz/pTTrack =0.5 * ( E - 1/E ) : constant along the helix */
  public final double tL;
  /** Transverse momentum * charge. */
  public final double pT;
  /** ln( Pz/pTTrack + SQRT[(Pz/pTTrack)**2 +1 ] */
  public final double eta;
  /** Angle between (0,0) and vertex as seen from the circle center [degrees]?
   *  For helices starting at PCA, startPhi=0. This is the case for reconstructed
   *  helices that are not yet associated to a secondary vertex or are
   *  drawn to PCA.
   *  Mutable because it is changed when tracks are used to fit a vertex. */
  double startPhi;

  // negative end vertex means none exits
  double rhoVertex = 0.0;
  private double rhoEndVertex = -1.0;

  private int numHits;

  double[][] cov=null;
  
  AOldHelix() {
      this(0, 0, 0, 0, 0);
  };
  
  /** Standard Helix constructor without end vertex */
  AOldHelix(float rhoVertex, float phiVertex, float zVertex,
      float pTTrack, float phiTrack, float etaTrack, int charge ) {
	  this(rhoVertex,phiVertex,zVertex,pTTrack,phiTrack,etaTrack,charge,-1);
  }
  
  /**
   * Standard Helix constructor with end vertex.
   * 
   * Note: this method is not called when creating helix representation of
   * reconstructed InDet tracks, but is called for simulated InDet tracks.
   *
   * @param rhoVertex rho of vertex from which track starts
   * @param phiVertex phi of vertex from which track starts
   * @param zVertex   Z of vertex from which track starts
   * @param pTTrack   transverse track momentum > 0
   * @param phiTrack  phi of track seen from vertex
   * @param etaTrack  eta = -ln(tg(theta/2)),  theta = arctg(pTTrack/Pz) = 2*arctg( e**(-eta))
   * @param charge    charge = +1 or -1
   * @param rhoEndVertex
   */
  AOldHelix(float rhoVertex, float phiVertex, float zVertex,
      float pTTrack, float phiTrack, float etaTrack, int charge, float rhoEndVertex ) {
	  /*
	   * Derived:
	   *  rC = signed radius of circle (positive for positive particle, which
	   *       loops in the clockwise direction in the standard xy projection,
	   *       given that Bz is positive in the ATLAS solenoid)
       *  xC = x position of center of circle
       *  yC = y position of center of circle
       *  E  = E**eta
       *  charge=1 if icode>=0 else charge=-1
       * 
       * Calculation:
       *  rC = C*pTTrack                            C = 100./0.6
       *  xC = rhoVertex*cos(phiVertex) + rC*sin(phiTrack)
       *  yC = rhoVertex*sin(phiVertex) - rC*cos(phiTrack)
       *  tl = Pz/pTTrack = 0.5 * ( E - 1/E )
       *  d0 = rC - sqrt(xC*xC + yC*yC)
       *  startPhi = pi/2 - phiTrack + atan2(yC,xC)    modify startPhi to: -pi/2 < startPhi < pi/2
       *  z0 = zVertex - rC*startPhi*tl
       *  phi0 = phiTrack + startPhi
       *  pCharge = charge*pTTrack
       *  Change startPhi and phi0 from radians to degrees.
       * 
       */
    double rC=C*pTTrack*charge;
    if (DEBUG) logger.debug("AOldHelix: charge, rC = "+charge+", "+rC);
    double xC=rhoVertex*Math.cos(phiVertex)+rC*Math.sin(phiTrack);
    double yC=rhoVertex*Math.sin(phiVertex)-rC*Math.cos(phiTrack);
    if (DEBUG) logger.debug("AOldHelix:  xCyC"+" "+xC+" "+yC);
    double e=Math.exp(etaTrack);
    tL=0.5*(e-1./e);
    eta = etaTrack;
    d0=rC-charge*Math.sqrt(xC*xC+yC*yC);
    phiTrack = (float) AMath.nearestPhiRadians(phiTrack);
    double temp=Math.atan2(yC, xC);
    temp = AMath.nearestPhiRadians(temp);
    double startPhiRad=charge*(PI/2)-phiTrack+temp;
    startPhiRad = AMath.nearestPhiRadians(startPhiRad, 0);
    z0=zVertex-rC*startPhiRad*tL;
    phi0=Math.toDegrees(phiTrack+startPhiRad);
    startPhi=charge*Math.toDegrees(startPhiRad);
    pT=charge*pTTrack;

    // keep rhoEndVertex for simulated tracks with a daughter
    this.rhoVertex=rhoVertex;
    this.rhoEndVertex = rhoEndVertex;
  }

  /**
   * Construct AOldHelix object.
   * @param d0
   * @param z0
   * @param phi0 [degrees]
   * @param tL
   * @param pT
   */
  AOldHelix(float d0, float z0, float phi0,
      float tL, float pT) {
    this.d0=d0;
    this.z0=z0;
    this.phi0=AMath.nearestPhiDegrees(phi0);
    this.tL=tL;
    this.eta = calculateEta(this.tL);
    this.pT=pT;
    startPhi=0.;
  }

  AOldHelix(float d0, float z0, float phi0,
      float tL, float pT, int numHits) {
    this(d0, z0, phi0, tL, pT);
    this.numHits=numHits;
  }

  AOldHelix(float d0, float z0, float phi0,
      float tL, float pT, float[][] c) {
    this.d0=d0;
    this.z0=z0;
    this.phi0=phi0;
    this.tL=tL;
    this.eta = calculateEta(this.tL);
    this.pT=pT;
    this.startPhi=0.;

    cov=new double[c.length][];
    if(c.length!=SIZE)
      logger.error("Helix error "+c.length);
    for(int i=0; i<c.length; ++i) {
      if(c[i].length!=SIZE)
        logger.error("Helix error "+c.length);
      cov[i]=new double[c[i].length];
    }
    for(int i=0; i<c.length; ++i)
      for(int j=0; j<c[i].length; ++j)
        cov[i][j]=c[i][j];
  }

  AOldHelix(float d0, float z0, float phi0,
      float tL, float pT, int numHits, float[][] c) {
    this(d0, z0, phi0, tL, pT, c);
    this.numHits=numHits;
  }

  public double[] getPar() {
    return new double[] {d0, z0, Math.toRadians(phi0), tL, 1./pT};
  }

  public double[][] getCov() {
    return cov;
  }

  public double getRhoEndVertex(){
      return rhoEndVertex;
  }
  
/**
 * Set Phi start so that the track will originate from the point on the
 * reconstructed track in the YX plane which is closest to the point (Vx,Vy).
 * Only used when drawing tracks originating from a reconstructed vertex.
 */
  public void setPhiStartByXYPoint(double Vx, double Vy) {
    //construct a local super object to get the xC and yC numbers
    ADHelix Dh=new ADHelix(this);
    // do the calculation to find the starting angle
    double Xc=Dh.getxC();
    double Yc=Dh.getyC();

    double a=-1.*(Vx-Xc);
    double b=-1.*(Vy-Yc);

    double pTTrack=pT;
    double phiTrack=Math.toRadians(phi0);

    double charge=1;
    if(pTTrack<0.0) charge=-1.;

    double temp=Math.atan2(b, a);
    temp = AMath.nearestPhiRadians(temp);
    double sPhi=charge*(PI/2)-phiTrack+temp;
    sPhi = AMath.nearestPhiRadians(sPhi, 0);
    sPhi=charge*Math.toDegrees(sPhi);
    startPhi=Math.abs(sPhi);
  }

  public String toString() {
	   int simpleOutput = AGlobals.instance().getSimpleOutput();
	   if(simpleOutput>0){
  		String output = "";
  		if(simpleOutput==1 || simpleOutput==3) 
  			output+= "\n PT="+String.format("%.3f",Math.abs(pT))+" GeV\n "+
  			AMath.ETA+" = "+String.format("%.3f",eta)+"\n "+
  			AMath.PHI+" = "+String.format("%.3f",phi0)+AMath.DEGREES;
      	if(simpleOutput==2 || simpleOutput==3)
      		output+= "\n Px="+String.format("%.3f",Math.abs(pT)*Math.cos(Math.toRadians(phi0)))+" GeV "+
      		"\n Py="+String.format("%.3f",Math.abs(pT)*Math.sin(Math.toRadians(phi0)))+" GeV "+
      		"\n Pz="+String.format("%.3f",Math.abs(pT)*Math.sinh(eta))+" GeV "+
      		"\n Charge = " + (int)(pT/Math.abs(pT));
      	return output;
  	  }
  	
    String s="";
    double f=1.;

    if(numHits>0)
      s+="\n numHits = "+numHits;
    // if(Math.abs(d0)<0.1) f=10000.;
    s+="\n d0 = "+String.format("%.3f",f*d0);
    if(cov!=null) s+=" "+AMath.PLUSMINUS+" "+String.format("%.3f",f*Math.sqrt(cov[0][0]));
    if(f>100)
      s+=" um";
    else
      s+=" cm";
    f=1.;
    // if(Math.abs(z0)<0.1) f=10000.;
    s+="\n z0 = "+String.format("%.3f",f*z0);
    if(cov!=null) s+=" "+AMath.PLUSMINUS+" "+String.format("%.3f",Math.sqrt(f*cov[1][1]));
    if(f>100)
      s+=" um";
    else
      s+=" cm";
  
    double zVtx = parameterStore.get("Event", "ZVtx").getD();
    //if(Math.abs(z0-zVtx) < 0.1) f=10000.;
    s+="\n |z0-zVtx| = "+String.format("%.3f",f*Math.abs(z0-zVtx));
    if(f>100)
      s+=" um";
    else
      s+=" cm";
    
    s+="\n phi0 = "+String.format("%.3f",phi0);
    if(cov!=null)
    {
        s += " " + AMath.PLUSMINUS + " " +
        String.format("%.3f",Math.toDegrees(Math.sqrt(cov[2][2]))) +
            AMath.DEGREES;
    }
    else
    {
        s += AMath.DEGREES;

    }
    s+= "  (" + String.format("%.3f",Math.toRadians(phi0));
    if(cov != null)
    {
        s += " " + AMath.PLUSMINUS + " " +
        String.format("%.3f",Math.toRadians(Math.sqrt(cov[2][2]))) + " rad)";
    }
    else
    {
        s += " rad)";
    }

    s+="\n "+AMath.ETA+" = "+String.format("%.3f",eta);
    if(cov!=null) {
      // based on the relation between eta and ctgTheta
      double etaError = Math.sqrt(cov[3][3] / (tL * tL + 1.0));
      s+=" "+AMath.PLUSMINUS+" "+String.format("%.3f",etaError);
    }
    s+="\n tL = "+String.format("%.3f",tL);
    if(cov!=null) s+=" "+AMath.PLUSMINUS+" "+String.format("%.3f",Math.sqrt(cov[3][3]));
    s+="\n pT = "+String.format("%.2f",pT) + " GeV";
    if(cov!=null) {
      double sigmaPt=Math.abs(1./(Math.abs(1./pT)+Math.sqrt(cov[4][4]))-Math.abs(pT));

      s+=" "+AMath.PLUSMINUS+" "+String.format("%.3f",sigmaPt)+" GeV";
    }
    if(pT<0.)
      s+="\n p  = "+String.format("%.2f",-AMath.getPFromPttL(pT,tL));
    else
      s+="\n p  = "+String.format("%.2f",AMath.getPFromPttL(pT,tL));
    s+= " GeV";
    
    return s;
  }

  public A3Vector getP() {
    double pTrans=Math.abs(pT);
    double phi=Math.toRadians(phi0);
    return new A3Vector(pTrans*Math.cos(phi), pTrans*Math.sin(phi), pTrans*tL);
  }

  /** eta = -ln(tg theta/2), so eta = -ln(sqrt(ctgtheta*ctgtheta+1)-ctgtheta) */
  private static double calculateEta(double ctgTheta) {
    double eta;
    eta = -Math.log(Math.sqrt(ctgTheta * ctgTheta + 1.0) - ctgTheta);
    return eta;
  }

}


package atlantis.geometry;

import atlantis.graphics.ACoord;
import atlantis.graphics.AClipper;
import atlantis.projection.AProjectionYX;
import atlantis.utils.AMath;

/**
 * Class representing a calorimeter part in the endcap section of ATLAS.
 */
public class AEndcapCalorimeterDetector extends ACalorimeterDetector {

    /**
     * Constructs a new endcap calorimeter object
     * @param name String name of the object
     * @param color String name of the color used to draw this object
     * @param sampling int sampling of this calorimeter part
     * @param region int region of this calorimeter part
     * @param rMin double inner radius
     * @param rMax double outer radius
     * @param zMin double inner z
     * @param zMax double outer z
     * @param eta0 double start of the first cell in eta
     * @param deta double size of a cell in eta
     * @param neta int number of cells in eta
     * @param meta int eta index of the first cell
     * @param phi0 double start of the first cell in phi
     * @param nphi int number of cells in phi
     */
    public AEndcapCalorimeterDetector(String name, String color, int sampling, int region,
                                      double rMin, double rMax, double zMin, double zMax,
                                      double eta0, double deta, int neta, int meta,
                                      double phi0, int nphi) {
        super(name, color);

        this.sampling = sampling;
        this.region = region;
        this.rMin = rMin;
        this.rMax = rMax;
        this.zMin = zMin;
        this.zMax = zMax;
        this.eta0 = eta0;
        this.deltaEta = deta;
        this.numEta = neta;
        this.minEta = meta;
        this.phi0 = Math.toRadians(phi0);
        this.numPhi = nphi;
        this.deltaPhi = 2.*Math.PI / numPhi;

        this.detEtaMin = AMath.etaAbs(this.zMin, this.rMax);
        this.detEtaMax = AMath.etaAbs(this.zMax, this.rMax);

        // This one is used for clipping the cell geometry in rho-z.
        this.clipper = new AClipper(zMin, zMax, rMin, rMax);

        // These arrays are used by the getDetectorIndex() method in ACalorimeterData.
        // They provide fast mapping of hits to detectors.
        if (this.name.indexOf("LAr") >= 0 && this.name.indexOf("Inner") >= 0 
            && this.sampling < lar[3].length && this.region < lar[3][this.sampling].length) {

           lar[3][this.sampling][this.region] = detectors.indexOf(this);
        } else if (this.name.indexOf("LAr") >= 0 && this.sampling < lar[2].length
            && this.region < lar[2][this.sampling].length) {

            lar[2][this.sampling][this.region] = detectors.indexOf(this);
        } else if (this.name.equals("HEC") && this.sampling < hec.length
                   && this.region < hec[this.sampling].length) {

            hec[this.sampling][this.region] = detectors.indexOf(this);
        } else {
            System.out.println("Unknown calorimeter object: '" + this.name + "'"
                               + " some hits might not be displayed correctly.");
        }
    }

    /**
     * Returns the y-x geometry of the full calorimeter part.
     * @return ACoord polygons representing geometry
     */
    protected ACoord getYXUser() {
        int mode = parameterStore.get("YX", "Mode").getI();
        if ( mode < AProjectionYX.MODE_LAR_ENDCAP_PRESAMPLER) 
            return ACoord.NO_DATA;
        else if((mode==AProjectionYX.MODE_LAR_ENDCAP_PRESAMPLER && (name.equals("LAr Endcap Presampler")||name.equals("LAr_EC_Presampler")))
             || (name.equals("LAr Outer Endcap") && sampling==mode-AProjectionYX.MODE_LAR_ENDCAP_PRESAMPLER)
             || (name.equals("LAr Inner Endcap") && sampling==mode-AProjectionYX.MODE_LAR_ENDCAP_PRESAMPLER-1)
             || (name.equals("HEC") && sampling==mode-AProjectionYX.MODE_HEC_1)
             || (mode==AProjectionYX.MODE_LAR_ENDCAP_SUMMED && name.indexOf("LAr")>=0)
             || (mode==AProjectionYX.MODE_HEC_SUMMED && name.equals("HEC")))
        {

            double etaMax = eta0 + numEta * deltaEta;
            double etaMin = eta0;
            double r1, r2;
            if (name.equals("HEC")) 
            {    
                r1=rMax;
                r2=rMin;
            }
            else
            {
                if (name.indexOf("Inner") >= 0 || name.indexOf("Presampler") >= 0)
                    r1 = zMin/Math.sinh(etaMin);
                else
                    r1=rMax;
                r2 = zMin/Math.sinh(etaMax);
            }
            
            if (!globals.getUseOpenGL()) {
                int numPoints = 2 * (numPhi + 1);
                double[][] hv = new double[2][numPoints];
                for (int i = 0; i < numPhi + 1; i++) {
                    hv[0][i] = r2 * Math.cos(phi0 + i * deltaPhi);
                    hv[1][i] = r2 * Math.sin(phi0 + i * deltaPhi);
                    hv[0][numPhi + 1 + i] = r1 * Math.cos(phi0 - i * deltaPhi);
                    hv[1][numPhi + 1 + i] = r1 * Math.sin(phi0 - i * deltaPhi);
                }
                return new ACoord(hv);
            } else {
                return new ACoord(generateAnnulusTris(numPhi, phi0, r1, r2));
            }
        }
        else
            return ACoord.NO_DATA;
    }
    
    /**
     * Returns the rho-z geometry of the full calorimeter part.
     * @return ACoord polygons representing geometry
     */
    protected ACoord getRZUser() {
        double h[], v[];
        double etaMax = eta0 + numEta * deltaEta;
        double etaMin = eta0;

        // Calculate the polygon in the positive h and v.
        if (name.indexOf("HEC") >= 0) {         
            h = new double[] {zMax, zMin, zMin, zMax};
            v = new double[] {rMax, rMax, rMin, rMin};          
        } else {
            double r0 = zMax/Math.sinh(etaMin);
            double r1 = zMin/Math.sinh(etaMin);
            double r2 = zMin/Math.sinh(etaMax);
            double r3 = zMax/Math.sinh(etaMax);
            
            h = new double[] {zMax, zMin, zMin, zMax};
            v = new double[] {r0, r1, r2, r3};
        }

        // Clip the polygon.
        ACoord coord = clipper.clipPolygon(h, v, h.length);
        
        // Copy it to negative v.
        coord = new ACoord(coord, coord.mirrorV());
        // Copy it to negative h.
        coord = new ACoord(coord, coord.mirrorH());

        return coord;
    }

    /**
     * Returns the phi-rho geometry of the full calorimeter part.
     * @return ACoord polygons representing geometry
     */
    protected ACoord getFRUser() {
        return getYXUser().convertYXToFR();
    }
    
    /**
     * Returns the phi-z geometry of the full calorimeter part.
     * @return ACoord polygons representing geometry
     */
    protected ACoord getFZUser() {
        double[][][] hv = new double[2][2][];
        double phiMin = 0.;
        double phiMax = 360.;
        hv[0][0] = new double[] {  zMax,   zMin,   zMin,   zMax};
        hv[1][0] = new double[] {phiMax, phiMax, phiMin, phiMin};
        hv[0][1] = new double[] { -zMax,  -zMin,  -zMin,  -zMax};
        hv[1][1] = new double[] {phiMax, phiMax, phiMin, phiMin};
        return new ACoord(hv);
    }

    /**
     * Returns the y-x geometry of a specific cell in this calorimeter part.
     * @param eta int eta index of cell
     * @param phi int phi index of cell
     * @return ACoord polygon representing cell geometry
     */
    public ACoord getYXCell(int eta, int phi) 
    {
        int mode = parameterStore.get("YX", "Mode").getI();
        if(mode < AProjectionYX.MODE_LAR_ENDCAP_PRESAMPLER || mode > AProjectionYX.MODE_HEC_SUMMED)
            return ACoord.NO_DATA;
        else if((mode==AProjectionYX.MODE_LAR_ENDCAP_SUMMED && name.indexOf("LAr")>=0)
                || (mode==AProjectionYX.MODE_HEC_SUMMED && name.equals("HEC")))
        {
        	//don't need to carry out calculation just needs return a blank ACoord
            double[][] hv = new double[2][];
            hv[0] = new double[] {0};
            hv[1] = new double[] {0};
            return new ACoord(hv);
        }
        else if((mode==AProjectionYX.MODE_LAR_ENDCAP_PRESAMPLER && (name.equals("LAr Endcap Presampler") || name.equals("LAr_EC_Presampler")))
            || (name.equals("LAr Outer Endcap") && sampling==mode-AProjectionYX.MODE_LAR_ENDCAP_PRESAMPLER)
            || (name.equals("LAr Inner Endcap") && sampling==mode-AProjectionYX.MODE_LAR_ENDCAP_PRESAMPLER-1)
            || (name.equals("HEC") && sampling==mode-AProjectionYX.MODE_HEC_1))            
        {
            double[][] hv = new double[2][];
            double phiMin = phi0+phi*deltaPhi;
            double phiMax = phiMin+deltaPhi;
            double newrMax=0, newrMin=0;
            double etaMin = eta0+(eta-minEta)*deltaEta;
            double etaMax = etaMin+deltaEta;
            newrMax=zMin/Math.sinh(etaMin);
            newrMin=zMin/Math.sinh(etaMax);
            if (name.equals("HEC"))
            {
                if(newrMax>rMax)
                    newrMax=rMax;
                if(newrMin<rMin)
                    newrMin=rMin;
            }
            hv[0] = new double[] {newrMax*Math.cos(phiMin), newrMax*Math.cos(phiMax),
                                  newrMin*Math.cos(phiMax), newrMin*Math.cos(phiMin)};
            hv[1] = new double[] {newrMax*Math.sin(phiMin), newrMax*Math.sin(phiMax),
                                  newrMin*Math.sin(phiMax), newrMin*Math.sin(phiMin)};
            return new ACoord(hv);
        }
        else
            return ACoord.NO_DATA;
    }

    /**
     * Returns the rho-z geometry of a specific cell in this calorimeter part.
     * @param eta int eta index of cell
     * @param phi int phi index of cell
     * @param side int sign of the z coordinate of cell
     * @return ACoord polygon representing cell geometry
     */
    public ACoord getRZCell(int eta, int phi, int side) {
        double[][] hv = new double[2][];
        int sign = getRSign(phi);

        // This shouldn't be 0, but just to be on the safe side.
        if (side != 0) side /= Math.abs(side);

        double etaMin = eta0+(eta-minEta)*deltaEta;
        double etaMax = etaMin+deltaEta;

        if (name.indexOf("HEC") >= 0) {  
            if (sampling == 0) {
                double rMin = (zMin+zMax)/2. / Math.sinh(etaMax);
                double rMax = (zMin+zMax)/2. / Math.sinh(etaMin);
    
                hv[0] = new double[] {zMax, zMin, zMin, zMax};
                hv[1] = new double[] {rMax, rMax, rMin, rMin};
            } else {
                double dz = (zMax-zMin);
                double z0 = zMin;
                double z1 = zMin+dz/2.;
                double z2 = zMax;
                double r0 = (zMin+dz/4.) / Math.sinh(etaMax);
                double r1 = (zMax-dz/4.) / Math.sinh(etaMax);
                double r2 = (zMin+dz/4.) / Math.sinh(etaMin);
                double r3 = (zMax-dz/4.) / Math.sinh(etaMin);
                
                hv[0] = new double[] {z2, z1, z1, z0, z0, z1, z1, z2};
                hv[1] = new double[] {r3, r3, r2, r2, r0, r0, r1, r1};
            }
        } else {
            hv[0] = new double[] {zMax, zMin, zMin, zMax};
            hv[1] = new double[] {zMax/Math.sinh(etaMin), zMin/Math.sinh(etaMin),
                                  zMin/Math.sinh(etaMax), zMax/Math.sinh(etaMax)};
        }

        // Clip the cell and send it to the right quadrant.
        ACoord coord = clipper.clipPolygon(hv[0], hv[1], hv[0].length);
        for (int i=0; i<coord.hv[0][0].length; i++) {
            coord.hv[0][0][i] *= side;
            coord.hv[1][0][i] *= sign;
        }

        return coord;
    }

    /**
     * Returns the phi-z geometry of a specific cell in this calorimeter part.
     * @param eta int eta index of cell
     * @param phi int phi index of cell
     * @param side int sign of the z coordinate of cell
     * @return ACoord
     */
    public ACoord getFZCell(int eta, int phi, int side) {
        double[][] hv = new double[2][];
        double phiMin = Math.toDegrees(phi0 + phi*deltaPhi);
        double phiMax = phiMin + Math.toDegrees(deltaPhi);

        side /= Math.abs(side);

        hv[0] = new double[] {side*zMax, side*zMin, side*zMin, side*zMax};
        hv[1] = new double[] {   phiMax,    phiMax,    phiMin,    phiMin};

        return new ACoord(hv);
    }

    /**
     * Returns the eta-phi geometry of a specific cell in this calorimeter part.
     * @param eta int eta index of cell
     * @param phi int phi index of cell
     * @param side int sign of the z coordinate of cell
     * @return ACoord polygon representing cell geometry
     */
    public ACoord getVPCell(int eta, int phi, int side) {
        double[][] hv = new double[2][];
        double etaMin = eta0+(eta-minEta)*deltaEta;
        double etaMax = etaMin+deltaEta;
        double phiMin = Math.toDegrees(phi0 + phi*deltaPhi);
        double phiMax = phiMin + Math.toDegrees(deltaPhi);

        side /= Math.abs(side);

        hv[0] = new double[] {side*etaMax, side*etaMin, side*etaMin, side*etaMax};
        hv[1] = new double[] {     phiMax,      phiMax,      phiMin,      phiMin};

        return new ACoord(hv);
    }
}

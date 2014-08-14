package atlantis.geometry;

import atlantis.graphics.ACoord;
import atlantis.graphics.AClipper;
import atlantis.projection.AProjectionYX;
import atlantis.utils.AMath;

/**
 * Class representing a detector part in the endcap cryostat section of ATLAS.
 */
public class AEndcapCryostatDetector extends ACalorimeterDetector {

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
     * @param neta int number of cells in eta
     * @param nphi int number of cells in phi
     */
    public AEndcapCryostatDetector(String name, String color, int sampling, int region, double rMin, 
                                      double rMax, double zMin, double zMax, int neta, int nphi) {
        super(name, color);

        this.sampling = sampling;
        this.region = region;
        this.rMin = rMin;
        this.rMax = rMax;
        this.zMin = zMin;
        this.zMax = zMax;
        this.numEta = neta;
        this.numPhi = nphi;
        this.deltaPhi = 2.*Math.PI / numPhi;
        this.phi0 = 0.0;
        this.eta0 = AMath.etaAbs(this.zMin, this.rMin);

        this.detEtaMin = AMath.etaAbs(this.zMin, this.rMax);
        this.detEtaMax = AMath.etaAbs(this.zMax, this.rMax);

        // This one is used for clipping the cell geometry in rho-z.
        this.clipper = new AClipper(zMin, zMax, rMin, rMax);

        // These arrays are used by the getDetectorIndex() method in ACalorimeterData.
        // They provide fast mapping of hits to detectors.
        if (this.name.equals("Minimum Bias Trigger Scintillators")) {
            mbts[this.sampling] = detectors.indexOf(this);
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
        if (mode!=AProjectionYX.MODE_MBTS) 
            return ACoord.NO_DATA;
        else
        {
            if (!globals.getUseOpenGL()) {
                int numPoints = 2 * (numPhi + 1);
                double[][] hv = new double[2][numPoints];
                for (int j = 0; j < numPhi + 1; j++) {
                    hv[0][j] = rMin * Math.cos(Math.PI * 2 * j / numPhi);
                    hv[1][j] = rMin * Math.sin(Math.PI * 2 * j / numPhi);
                }
                for (int j = 0; j < numPhi + 1; j++) {
                    hv[0][j + numPhi + 1] = rMax * Math.cos(Math.PI * 2 * (numPhi - j) / numPhi);
                    hv[1][j + numPhi + 1] = rMax * Math.sin(Math.PI * 2 * (numPhi - j) / numPhi);
                }
                return new ACoord(hv);
            } else {
                return new ACoord(generateAnnulusTris(numPhi, phi0, rMin, rMax));
            }
        }
    }
    
    /**
     * Returns the rho-z geometry of the full calorimeter part.
     * @return ACoord polygons representing geometry
     */
    protected ACoord getRZUser() {
        double h[], v[];
        // Calculate the polygon in the positive h and v.
        h = new double[] {zMax, zMin, zMin, zMax};
        v = new double[] {rMax, rMax, rMin, rMin};          
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
     * Returns the y-x geometry of a specific cell in this calorimeter part.
     * @param eta int eta index of cell
     * @param phi int phi index of cell
     * @return ACoord polygon representing cell geometry
     */
    public ACoord getYXCell(int eta, int phi) 
    {
        int mode = parameterStore.get("YX", "Mode").getI();
        if(mode!=AProjectionYX.MODE_MBTS)
            return ACoord.NO_DATA;
        else            
        {
            double[][] hv = new double[2][];
            double phiMin = Math.PI*2*phi/numPhi;
            double phiMax = Math.PI*2*(1+phi)/numPhi;
            hv[0] = new double[] {rMax*Math.cos(phiMin), rMax*Math.cos(phiMax),
                                  rMin*Math.cos(phiMax), rMin*Math.cos(phiMin)};
            hv[1] = new double[] {rMax*Math.sin(phiMin), rMax*Math.sin(phiMax),
                                  rMin*Math.sin(phiMax), rMin*Math.sin(phiMin)};
            return new ACoord(hv);
        }
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
        hv[0] = new double[] {zMax, zMin, zMin, zMax};
        hv[1] = new double[] {rMax, rMax, rMin, rMin};
        // Clip the cell and send it to the right quadrant.
        ACoord coord = clipper.clipPolygon(hv[0], hv[1], hv[0].length);
        for (int i=0; i<coord.hv[0][0].length; i++) {
            coord.hv[0][0][i] *= side;
            coord.hv[1][0][i] *= sign;
        }
        return coord;
    }
}

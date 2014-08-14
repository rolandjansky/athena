package atlantis.geometry;

import atlantis.graphics.ACoord;
import atlantis.parameters.AParameterUtilities;

/**
 * Class representing a calorimeter in the gap region between TILE and extended TILE.
 * This is a fairly straightforward implementation of ACalorimeterDetector with only
 * one cell per object.
 *
 * @author Eric Jansen
 */
public class AGapCalorimeterDetector extends ACalorimeterDetector {

    protected int eta;

    /**
     * Constructs a new gap calorimeter object (one cell in eta).
     * @param name String name of the object
     * @param color String name of the color used to draw this object
     * @param sampling int sampling of this calorimeter part
     * @param region int region of this calorimeter part
     * @param rMin double inner radius
     * @param rMax double outer radius
     * @param zMin double inner z
     * @param zMax double outer z
     * @param eta int eta index of this cell
     * @param phi0 double start of the first cell in phi
     * @param nphi int number of cells in phi
     */
    public AGapCalorimeterDetector(String name, String color, int sampling, int region,
                                   double rMin, double rMax, double zMin, double zMax,
                                   int eta, double phi0, int nphi) {
        super(name, color);

        // Just to be sure we take absolute values here.
        this.sampling = Math.abs(sampling);
        this.region = Math.abs(region);
        this.eta = Math.abs(eta);

        this.rMin = rMin;
        this.rMax = rMax;
        this.zMin = zMin;
        this.zMax = zMax;
        this.phi0 = Math.toRadians(phi0);
        this.numPhi = nphi;
        this.deltaPhi = 2.*Math.PI/numPhi;

        // These arrays are used by the getDetectorIndex() method in ACalorimeterData.
        // They provide fast mapping of hits to detectors.
        if (this.name.indexOf("ITC") >= 0 && this.sampling < tile_gap.length
            && this.eta < tile_gap[this.sampling].length) {

            tile_gap[this.sampling][this.eta] = detectors.indexOf(this);
        } else {
            System.out.println("Unknown calorimeter object: '" + this.name + "'"
                               + " some hits might not be displayed correctly.");
        }
    }

    /**
     * Returns the eta value of the center (in eta) of a cell.
     * @param eta int eta index of the cell
     * @param side int sign of the z coordinate of the cell
     * @return double eta value
     */
    public double getEta(int eta, int side) {
        if (side < 0) {
            return -getEta(eta, -side);
        } else {
            return AParameterUtilities.eta((zMin+zMax)/2.f, (rMin+rMax)/2.f);
        }
    }

    /**
     * Returns the lowest eta value of a cell.
     * @param eta int eta index of the cell
     * @param side int sign of the z coordinate of the cell
     * @return double lowest eta value
     */
    public double getEtaMin(int eta, int side) {
        if (side < 0) {
            return -getEtaMax(eta, -side);
        } else {
            return AParameterUtilities.eta((zMin+zMax)/2.f, rMax);
        }
    }

    /**
     * Returns the highest eta value of a cell.
     * @param eta int eta index of the cell
     * @param side int sign of the z coordinate of the cell
     * @return double highest eta value
     */
    public double getEtaMax(int eta, int side) {
        if (side < 0) {
            return -getEtaMin(eta, -side);
        } else {
            return AParameterUtilities.eta((zMin+zMax)/2.f, rMin);
        }
    }

    /**
     * Returns the rho-z geometry of the full calorimeter part.
     * @return ACoord polygons representing geometry
     */
    protected ACoord getRZUser() {
        double[][][] hv;
        if (zMin < 0) {
            hv = new double[2][2][];
            hv[0][0] = new double[] { zMax,  zMin,  zMin,  zMax};
            hv[1][0] = new double[] { rMax,  rMax,  rMin,  rMin};
            hv[0][1] = new double[] { zMax,  zMin,  zMin,  zMax};
            hv[1][1] = new double[] {-rMin, -rMin, -rMax, -rMax};
        } else {
            hv = new double[2][4][];
            hv[0][0] = new double[] { zMax,  zMin,  zMin,  zMax};
            hv[1][0] = new double[] { rMax,  rMax,  rMin,  rMin};
            hv[0][1] = new double[] { zMax,  zMin,  zMin,  zMax};
            hv[1][1] = new double[] {-rMin, -rMin, -rMax, -rMax};
            hv[0][2] = new double[] {-zMin, -zMax, -zMax, -zMin};
            hv[1][2] = new double[] { rMax,  rMax,  rMin,  rMin};
            hv[0][3] = new double[] {-zMin, -zMax, -zMax, -zMin};
            hv[1][3] = new double[] {-rMin, -rMin, -rMax, -rMax};
        }
        return new ACoord(hv);
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
        if (side != 0) {
            side /= Math.abs(side);
        }

        hv[0] = new double[] {zMax, zMin, zMin, zMax};
        hv[1] = new double[] {rMax, rMax, rMin, rMin};

        // Send it to the right quadrant.
        ACoord coord = new ACoord(hv);
        for (int i=0; i<coord.hv[0][0].length; i++) {
            coord.hv[0][0][i] *= side;
            coord.hv[1][0][i] *= sign;
        }

        return coord;
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
        double etaMin = AParameterUtilities.eta((zMin+zMax)/2.f, rMax);
        double etaMax = AParameterUtilities.eta((zMin+zMax)/2.f, rMin);
        double phiMin = Math.toDegrees(phi0 + phi*deltaPhi);
        double phiMax = phiMin + Math.toDegrees(deltaPhi);

        side /= Math.abs(side);

        hv[0] = new double[] {side*etaMax, side*etaMin, side*etaMin, side*etaMax};
        hv[1] = new double[] {     phiMax,      phiMax,      phiMin,      phiMin};

        return new ACoord(hv);
    }
}

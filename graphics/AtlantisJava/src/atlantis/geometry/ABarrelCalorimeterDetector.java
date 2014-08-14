package atlantis.geometry;

import atlantis.graphics.ACoord;
import atlantis.graphics.AClipper;
import atlantis.projection.AProjectionYX;
import atlantis.utils.AMath;

/**
 * Class representing a calorimeter part in the barrel section of ATLAS.
 *
 * @author Eric Jansen
 */
public class ABarrelCalorimeterDetector extends ACalorimeterDetector {

    /**
     * Constructs a new barrel calorimeter object.
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
    public ABarrelCalorimeterDetector(String name, String color, int sampling, int region,
            double rMin, double rMax, double zMin, double zMax,
            double eta0, double deta, int neta, int meta,
            double phi0, int nphi) {
        super(name, color);

        // Just to be sure we take absolute values here.
        this.sampling = Math.abs(sampling);
        this.region = Math.abs(region);

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
        this.deltaPhi = 2.*Math.PI/numPhi;

        this.detEtaMin = AMath.etaAbs(this.zMax, this.rMax);
        this.detEtaMax = AMath.etaAbs(this.zMin, this.rMin);

        if (this.eta0 <= 0.0) {
            this.zMin = -this.zMax;
            this.detEtaMin = -this.detEtaMax;
        }

        // This one is used for clipping the cell geometry in rho-z.
        this.clipper = new AClipper(this.zMin, this.zMax, this.rMin, this.rMax);

        // These arrays are used by the getDetectorIndex() method in ACalorimeterData.
        // They provide fast mapping of hits to detectors.
        if (this.name.indexOf("LAr") >= 0 && this.sampling < lar[1].length
            && this.region < lar[1][this.sampling].length) {

            lar[1][this.sampling][this.region] = detectors.indexOf(this);
        } else if (this.name.equals("TILE Barrel") && this.sampling < tile.length) {
            tile[this.sampling] = detectors.indexOf(this);

        } else if (this.name.equals("Extended TILE") && this.sampling < tile_ext.length) {
            tile_ext[this.sampling] = detectors.indexOf(this);

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
        if (!globals.getUseOpenGL()) {
            int numPoints = 2 * (numPhi + 1);
            double[][] hv = new double[2][numPoints];
            for (int i = 0; i < numPhi + 1; i++) {
                hv[0][i] = rMin * Math.cos(phi0 + i * deltaPhi);
                hv[1][i] = rMin * Math.sin(phi0 + i * deltaPhi);
                hv[0][numPhi + 1 + i] = rMax * Math.cos(phi0 - i * deltaPhi);
                hv[1][numPhi + 1 + i] = rMax * Math.sin(phi0 - i * deltaPhi);
            }
            return new ACoord(hv);
        } else {
            return new ACoord(generateAnnulusTris(numPhi, phi0, rMin, rMax));
        }
    }

    /**
     * Returns the rho-z geometry of the full calorimeter part.
     * @return ACoord polygons representing geometry
     */
    protected ACoord getRZUser() {
        double[] h, v;

        double etaMax = eta0 + numEta * deltaEta;
        double etaMin;
        if (eta0 <= 0.0) {
            etaMin = -etaMax;
        } else {
            etaMin = eta0;
        }

        // Calculate the polygon in the positive h and v.
        if (name.indexOf("TILE") >= 0) {
            h = new double[] {zMax, zMin, zMin, zMax};
            v = new double[] {rMax, rMax, rMin, rMin};
        } else {
            double z0 = rMax*Math.sinh(etaMax);
            double z1 = rMax*Math.sinh(etaMin);
            double z2 = rMin*Math.sinh(etaMin);
            double z3 = rMin*Math.sinh(etaMax);

            h = new double[] {z0, z1, z2, z3};
            v = new double[] {rMax, rMax, rMin, rMin};
        }

        // Clip the polygon.
        ACoord coord = clipper.clipPolygon(h, v, h.length);

        // Copy it to negative v.
        coord = new ACoord(coord, coord.mirrorV());
        if (eta0 > 0.0) {
            // If it doesn't continue through h=0, copy it to negative h.
            coord = new ACoord(coord, coord.mirrorH());
        }

        return coord;
    }

    /**
     * Returns the phi-rho geometry of the full calorimeter part.
     * @return ACoord polygons representing geometry
     */
    protected ACoord getFRUser() {
        double[][] hv = new double[2][];

        hv[0] = new double[] {rMin, rMax, rMax, rMin};
        hv[1] = new double[] {360., 360., 0., 0.};

        return new ACoord(hv);
    }

    /**
     * Returns the y-x geometry of a specific cell in this calorimeter part.
     * @param eta int eta index of cell
     * @param phi int phi index of cell
     * @return ACoord polygon representing cell geometry
     */
    public ACoord getYXCell(int eta, int phi) {
        if (parameterStore.get("YX", "Mode").getI() != AProjectionYX.MODE_STANDARD) return ACoord.NO_DATA;
        double[][] hv = new double[2][];
        double phiMin = phi0+phi*deltaPhi;
        double phiMax = phiMin+deltaPhi;

        hv[0] = new double[] {rMax*Math.cos(phiMin), rMax*Math.cos(phiMax),
                              rMin*Math.cos(phiMax), rMin*Math.cos(phiMin)};
        hv[1] = new double[] {rMax*Math.sin(phiMin), rMax*Math.sin(phiMax),
                              rMin*Math.sin(phiMax), rMin*Math.sin(phiMin)};

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

        if (side != 0) side /= Math.abs(side);

        if (name.indexOf("TILE") >= 0) {
            // Outer layer of TILE has eta increments of two.
            if (sampling == 2) eta = minEta + (eta-minEta)/2;

            double etaMin = eta0+(eta-minEta)*deltaEta;
            double etaMax = etaMin+deltaEta;

            if (name.indexOf("Barrel") >= 0 && sampling == 1) {
                double dr = (rMax-rMin);
                double r0 = rMin;
                double r1 = rMin+dr/2.;
                double r2 = rMax;
                double z0 = (rMin+dr/4.) * Math.sinh(etaMin);
                double z1 = (rMax-dr/4.) * Math.sinh(etaMin);
                double z2 = (rMin+dr/4.) * Math.sinh(etaMax);
                double z3 = (rMax-dr/4.) * Math.sinh(etaMax);

                if (eta == this.minEta && this.zMin > 0) z0 = z1 = this.zMin;
                if (eta == this.minEta + this.numEta - 1) z2 = z3 = this.zMax;

                hv[0] = new double[] {z3, z1, z1, z0, z0, z2, z2, z3};
                hv[1] = new double[] {r2, r2, r1, r1, r0, r0, r1, r1};
            } else {
                double zMin = (rMin+rMax)/2. * Math.sinh(etaMin);
                double zMax = (rMin+rMax)/2. * Math.sinh(etaMax);

                if (eta == this.minEta && this.zMin > 0)zMin = this.zMin;
                if (eta == this.minEta + this.numEta - 1) zMax = this.zMax;

                hv[0] = new double[] {zMax, zMin, zMin, zMax};
                hv[1] = new double[] {rMax, rMax, rMin, rMin};
            }
        } else if (name.indexOf("LAr") >= 0) {
            double etaMin = eta0+(eta-minEta)*deltaEta;
            double etaMax = etaMin+deltaEta;

            hv[0] = new double[] {rMax*Math.sinh(etaMax), rMax*Math.sinh(etaMin),
                                  rMin*Math.sinh(etaMin), rMin*Math.sinh(etaMax)};
            hv[1] = new double[] {rMax, rMax, rMin, rMin};
        } else {
            return ACoord.NO_DATA;
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
            if (name.indexOf("TILE") >= 0) {
                // Outer layer of TILE has eta increments of two.
                if (sampling == 2) eta = minEta + (eta-minEta)/2;
            }

            return eta0 + (eta - minEta + 0.5) * deltaEta;
        }
    }
}

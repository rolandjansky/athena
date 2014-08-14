package atlantis.utils;

/**
 * Provides basic 4D vector operations.
 */
public class A4Vector
{
    public double px, py, pz, e;

    public A4Vector()
    {}

    public A4Vector(double dx, double dy, double dz, double m)
    {
        set(dx, dy, dz, m);
    }

    public A4Vector(A3Vector v, double m)
    {
        set(v.x, v.y, v.z, m);
    }

    public void set(double dx, double dy, double dz, double m)
    {
        px = dx;
        py = dy;
        pz = dz;
        e = Math.sqrt(px * px + py * py + pz * pz + m * m);
    }

    public void setPtEtaPhiM(double pT, double eta, double phi, double mass)
    {
        double ptAbs = Math.abs(pT);
        px = ptAbs*Math.cos(phi);
        py = ptAbs*Math.sin(phi);
        pz = ptAbs*Math.sinh(eta);
        e = Math.sqrt(px * px + py * py + pz * pz + mass * mass);
    }

    public double getP()
    {
        return Math.sqrt(px * px + py * py + pz * pz);
    }

    public double getPt()
    {
        return Math.sqrt(px * px + py * py);
    }

    public double getE()
    {
        return e;
    }

    /**
     * getEt() returns the transverse energy.
     * The formula used to calculate it is:
     * Et^2 = E^2 * Pt^2 / P^2.
     */
    public double getEt()
    {
    	double pt2 = px * px + py * py;
    	double p2 = pt2 + pz * pz;
    	return Math.sqrt(e * e * pt2/p2);
    }

    public double getMass()
    {
        return Math.sqrt(e * e - px * px - py * py - pz * pz);
    }

    /**
     * getMt() returns the transverse mass.
     * The formula used to calculate it is:
     * Mt^2 = Et^2 - Pt^2.
     * 
     * This is from the Particle Data Group (http://pdg.lbl.gov/)
     * 
     * Note this is different to another definition in the PDG:
     * Mt^2 = E^2 - Pz^2 = m^2 + Pt^2
     *   
     * [J. Beringer et al. (Particle Data Group), Phys. Rev. D86, 010001 (2012).
     * (Kinematics, 43.6.1 and 43.5.2)]
     */
    public double getMt()
    {
    	double pt2 = px * px + py * py;
    	double Et = getEt();
    	return Math.sqrt(Et * Et - pt2);
    }

    public double getPhi()
    {
    	return px == 0.0 && py == 0.0 ? 0.0 : Math.atan2(py,px);
    }

    public double getEta()
    {
    	double ptot = getP();
    	double cosTheta = ptot == 0.0 ? 1.0 : pz/ptot;
    	if (cosTheta*cosTheta < 1) return -0.5* Math.log( (1.0-cosTheta)/(1.0+cosTheta) );
    	if (pz > 0) return 10e10;
    	else        return -10e10;
    }

    public void add(double dx, double dy, double dz, double m)
    {
        px += dx;
        py += dy;
        pz += dz;
        e += Math.sqrt(dx * dx + dy * dy + dz * dz + m * m);
    }

    public void add(A4Vector v)
    {
        px += v.px;
        py += v.py;
        pz += v.pz;
        e += v.e;
    }

    public String toString()
    {
        return "A4Vector[px=" + px + ", py=" + py + ", pz=" + pz + ", E=" + e + ", m=" + getMass() + "]";
    }

}

package atlantis.data;

import atlantis.event.*;
import atlantis.globals.AGlobals;
import atlantis.utils.AHashMap;
import atlantis.utils.AMath;
import atlantis.utils.A3Vector;
import atlantis.utils.A4Vector;

/**
 * Reconstructed B-Jet
 */
public class ABJetData extends AAODData
{
    private float[] lhSig;
    private float[] weight;
    private String[] label;
    private float[] mass;
    //Variable to ensure backwards compatability
    protected boolean has4Vect = false;

    ABJetData(AHashMap p, AEvent e)
    {
        super(p,e);
        lhSig=p.getFloatArray("lhSig");
        weight=p.getFloatArray("weight");
        label=p.getUnsureStringArray("label");
        if (p.get("mass") != null) {
          mass=p.getFloatArray("mass"); 
          has4Vect = true;
        }
    }

    public String getParameterGroup()
    {
        return "BJet";
    }

    public float getweight(int index)
    {
        return weight[index];
    }

    public float getlhSig(int index)
    {
        return lhSig[index];
    }
    
    public String getLabel(int index)
    {
    	return label[index];
    }

        public A4Vector get4Vector(int num, int[] list)
    {
        A4Vector sum = new A4Vector();
        if(has4Vect){
            for (int i =    0; i < num; ++i)
            {
                int k = list[i];
                A4Vector start = new A4Vector();
                start.setPtEtaPhiM(pT[k],eta[k],phi[k],mass[k]);
                sum.add(start);
            }
        }else{
            for (int i =    0; i < num; ++i)
            {
                int k = list[i];
                A3Vector start = A3Vector.fromRhoPhiZ(0., 0., 0.);
                double tL = AMath.tanLambda(eta[k]);
                A3Vector stop = A3Vector.fromRhoPhiZ(1., phi[k], tL);
                A3Vector v = (stop.subtract(start)).normalize();
                double p = pT[k] / Math.sqrt(1. - v.z * v.z);
                sum.add(new A4Vector(v.scale(p), 0.));
            }
        }
        return sum;
    }


    protected void applyCuts()
    {
        super.applyCuts();
        cut("CutsObjects", "BJetPt", " |ET|", pT);
        cut("CutsObjects", "BJetlhSig", " |lhSig|", lhSig);
        cut("CutsObjects", "BJetweight", " |weight|", weight);
    }

    public String getHitInfo(int index)
    {
    	int simpleOutput = AGlobals.instance().getSimpleOutput();
    	if(simpleOutput>0){
    		String output = getNameScreenName()+" index: " + index;
    		if(simpleOutput==1 || simpleOutput==3) 
    			output+= "\n PT="+String.format("%.3f",pT[index])+" GeV\n "+
    			AMath.ETA+" = "+String.format("%.3f",eta[index])+"\n "+
    			AMath.PHI+" = "+String.format("%.3f",phi[index])+AMath.DEGREES;
        	if(simpleOutput==2 || simpleOutput==3) 
        		output+= "\n Px="+String.format("%.3f",pT[index]*Math.cos(phi[index]))+" GeV "+
        		"\n Py="+String.format("%.3f",pT[index]*Math.sin(phi[index]))+" GeV "+
        		"\n Pz="+String.format("%.3f",pT[index]*Math.sinh(eta[index]))+" GeV ";
        	return output;
    	}

    	StringBuffer msg = new StringBuffer(getNameScreenName());
        msg.append(" (id: " + id[index] + " index: " + index + ")");
        if(label != null)
        {
        	msg.append("\n label = ");
        	msg.append(label[index]);
        }
        msg.append("\n PT = ");
        msg.append(String.format("%.3f",pT[index]));
        msg.append(" GeV\n P = ");
        msg.append(String.format("%.3f",Math.abs(pT[index]/Math.cos(AMath.lambda(eta[index])))));
        msg.append(" GeV\n ");
        msg.append(AMath.ETA);
        msg.append(" = ");
        msg.append(String.format("%.3f",eta[index]));
        msg.append("\n ");
        msg.append(AMath.PHI);
        msg.append(" = ");
        msg.append(String.format("%.3f",phi[index]));
        msg.append(AMath.DEGREES);
        msg.append("  (" + String.format("%.3f",phi[index]) + " rad)");
        msg.append("\n lhSig = ");
        msg.append(lhSig[index]);
        msg.append("\n weight = ");
        msg.append(weight[index]);

        return msg.toString();
    }

}

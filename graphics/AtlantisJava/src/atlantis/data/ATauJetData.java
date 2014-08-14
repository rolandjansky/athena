package atlantis.data;

import java.util.Vector;

import atlantis.event.*;
import atlantis.globals.AGlobals;
import atlantis.utils.AHashMap;
import atlantis.utils.AMath;
import atlantis.utils.A4Vector;

/**
 * Reconstructed Tau Jet
 */
public class ATauJetData extends AAODData
{
    // Note: This is a hierarchy list ! I.e. TauCutTight is assumed to have passed
    //   all other criteria as well. This is strictly not completely correct but 
    //   good enough for Atlantis purposes.
	private enum isTau { TauCutTight, TauCutMedium, TauCutLoose, TauLlhTight, TauLlhMedium, TauLlhLoose, none }
	
    private float[] charge;
    private int[] integerCharge;
    private int[] numTracks;
    private float[] isolFrac;
    private float[] logLhRatio;
    private String[] label;
    private Vector<Enum> isTauEnum = new Vector<Enum>();

    ATauJetData(AHashMap p, AEvent e)
    {
        super(p,e);
        charge=p.getFloatArray("charge");
        integerCharge = new int[charge.length];

        // The recommended default cut for Tau charge is to equal to 1/-1,
        // but the cut for float value has no "=" operator in the
        // implementation, so change float[] to int[] to make it possible
        // to use "=".
        for(int i=0; i<charge.length; ++i)
        {
            integerCharge[i] = (int) charge[i];
        }
        numTracks=p.getUnknownIntArray("numTracks");
        isolFrac=p.getUnsureFloatArray("isolFrac");
        logLhRatio=p.getUnsureFloatArray("logLhRatio");
        label=p.getUnsureStringArray("label");
        String[] isTauString=p.getUnsureStringArray("isTauString");
        
        if(isTauString!=null){
        	for(String s : isTauString)
        	{
        		try {
        			isTauEnum.add(isTau.valueOf(s));
        		}
        		catch(IllegalArgumentException q) {
        			isTauEnum.add(isTau.none);
        		}
        	}
        }
        else
        {
        	for(int i=0;i<numData;i++)
        	{
        		isTauEnum.add(isTau.none);
        	}
        }
    }

    public String getParameterGroup()
    {
        return "TauJet";
    }

    public String getName()
    {
        return "TauJet";
    }

    public float getCharge(int index)
    {
        return charge[index];
    }

    public int getNumTracks(int index)
    {
        return numTracks[index];
    }

    public float getIsolFrac(int index)
    {
        if(isolFrac != null) return isolFrac[index];
        else return -100;
    }

    public float getLogLhRatio(int index)
    {
        if(logLhRatio != null) return logLhRatio[index];
        else return -100;
    }
    
    public String getisTau(int index)
    {
        return isTauEnum.get(index).toString();
    }

    public A4Vector get4Vector(int num, int[] list)
    {
        A4Vector sum = new A4Vector();
        for (int i = 0; i < num; ++i)
        {
            int k = list[i];
            A4Vector start = new A4Vector();
            start.setPtEtaPhiM(pT[k],eta[k],phi[k],1.77682);
            sum.add(start);
        }

        return sum;
    }

    protected void applyCuts()
    {
        super.applyCuts();
        cut("CutsObjects", "TauJetPt", " |ET|", pT);
        cut("CutsObjects", "TauJetCharge", " |Charge|", integerCharge);
        cut("CutsObjects", "TauJetNumTracks", " |NumTracks|", numTracks);
        if(isolFrac != null)
            cut("CutsObjects", "TauJetisolFrac", " isolFrac", isolFrac);
        if(logLhRatio != null)
            cut("CutsObjects", "TauJetlogLhRatio", " logLhRatio", logLhRatio);

        if (parameterStore.get("CutsObjects", "TauJetisTauString").getStatus())
        {
        	int cutSub = parameterStore.get("CutsObjects", "TauJetisTauString").getI();
        	cutArrayEnum(isTauEnum, cutSub, "TauJet isTauString");
        }
    }

    public String getHitInfo(int index)
    {
    	int simpleOutput = AGlobals.instance().getSimpleOutput();
    	if(simpleOutput>0){
    		String output = getNameScreenName()+" index: " + index;
    		if(simpleOutput==1 || simpleOutput==3) 
    			output+= "\n PT="+String.format("%.3f",pT[index])+" GeV\n "+
    			AMath.ETA+" = "+String.format("%.3f",eta[index])+"\n "+
    			AMath.PHI+" = "+String.format("%.3f",Math.toDegrees(phi[index]))+AMath.DEGREES;
        	if(simpleOutput==2 || simpleOutput==3) 
        		output+= "\n Px="+String.format("%.3f",pT[index]*Math.cos(phi[index]))+" GeV "+
        		"\n Py="+String.format("%.3f",pT[index]*Math.sin(phi[index]))+" GeV "+
        		"\n Pz="+String.format("%.3f",pT[index]*Math.sinh(eta[index]))+" GeV ";
        	return output;
    	}
    	
        String k = this.getStoreGateKey();
        String sgKey = k != null ? k : "n/a";
        StringBuffer msg = new StringBuffer(getNameScreenName());
        msg.append(" (id: " + id[index] + " index: " + index + ")");
        msg.append("\n storegate key: ");
        msg.append(sgKey);
        msg.append("\n PT = ");
        msg.append(String.format("%.3f",pT[index]));
        msg.append(" GeV\n P = ");
        msg.append(String.format("%.3f",Math.abs(pT[index]/Math.cos(AMath.lambda(eta[index])))));
        msg.append(" GeV\n Charge = ");
        msg.append(integerCharge[index]);
        msg.append("\n ");
        msg.append(AMath.ETA);
        msg.append(" = ");
        msg.append(String.format("%.3f",eta[index]));
        msg.append("\n ");
        msg.append(AMath.PHI);
        msg.append(" = ");
        msg.append(String.format("%.3f",Math.toDegrees(phi[index])));
        msg.append(AMath.DEGREES);
        msg.append("  (" + String.format("%.3f",phi[index]) + " rad)");
        msg.append("\n numTracks = ");
        msg.append(numTracks[index]);
        
        if (label != null)
        {
            msg.append("\n label = ");
            msg.append(label[index]);
        }
        if (isolFrac != null)
        {
            msg.append("\n isolFrac = ");
            msg.append(isolFrac[index]);
        }
        if (logLhRatio != null)
        {
            msg.append("\n logLhRatio = ");
            msg.append(logLhRatio[index]);
        }

        return msg.toString();
    }

}

package atlantis.data;

import java.util.Vector;
import atlantis.utils.A4Vector;

import atlantis.event.*;
import atlantis.globals.AGlobals;
import atlantis.utils.AHashMap;
import atlantis.utils.AMath;

/**
 * Reconstructed Electron
 */
public class AElectronData extends AHelixAODData
{	
	private enum isEMe  { TightPP, Tight, Medium, Loose, none }
	
    private int[] isEM;
    private Vector<Enum> isEMEnum = new Vector<Enum>();
    private String[] label;
    private float[] eOverp;
    private int[] hasTrack;
    
    AElectronData(AHashMap p, AEvent e)
    {
        super(p,e);
        isEM=p.getUnsureIntArray("isEM");
        String[] isEMString=p.getUnsureStringArray("isEMString");
        eOverp = p.getUnsureFloatArray("eOverp");
        hasTrack=p.getUnsureIntArray("hasTrack");
        label=p.getUnsureStringArray("label");
        
        if(isEMString!=null)
        {
        	for(String s : isEMString)
        	{
        		try {
        			isEMEnum.add(isEMe.valueOf(s));
        		}
        		catch(IllegalArgumentException q) {
        			isEMEnum.add(isEMe.none);
        		}
        	}
        }
        else
        {
        	for(int i=0;i<numData;i++)
        	{
        		isEMEnum.add(isEMe.none);
        	}
        }
    }

    public String getParameterGroup()
    {
        return "Electron";
    }

    public float geteOverp(int index)
    {
        return eOverp[index];
    }

    public String getisEMEnum(int index)
    {
    	return isEMEnum.get(index).toString();
    }

    public int getisEM(int index)
    {
        return isEM[index];
    }
    
    public A4Vector get4Vector(int num, int[] list)
    {
        A4Vector sum = new A4Vector();
        for (int i = 0; i < num; ++i)
        {
            int k = list[i];
            A4Vector start = new A4Vector();
            start.setPtEtaPhiM((double)pT[k],(double)eta[k],(double)phi[k],0.00051);
            sum.add(start);
        }

        return sum;
    }

    protected void applyCuts()
    {
        super.applyCuts();
        cut("CutsObjects", "ElectronPt", " |ET|", pT);
        
        // because of the retriever bug, currently 2048 means 0 for isEM
        // remove this for block when the bug is fixed
        if(isEM != null)
        {
            for(int i=0; i<isEM.length; i++)
            {
                if(isEM[i]==2048)
                    isEM[i] = 0;
            }
        }
        
        cut("CutsObjects", "ElectronisEM", " isEM", isEM);

        if (parameterStore.get("CutsObjects", "ElectronisEMString").getStatus())
        {
        	int cutSub = parameterStore.get("CutsObjects", "ElectronisEMString").getI();
        
        	cutArrayEnum(isEMEnum, cutSub, "Electron isEMString");
        }
        
        cut("CutsObjects", "ElectroneOverp", " |eOverp|", eOverp);
        if (parameterStore.get("CutsObjects", "ElectronhasTrack").getStatus() && hasTrack!=null)
            cutArray(hasTrack, 1, "hasTrack");
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
        msg.append(" GeV\n ");
        msg.append(AMath.ETA);
        msg.append(" = ");
        msg.append(String.format("%.3f",eta[index]));
        msg.append("\n ");
        msg.append(AMath.PHI);
        msg.append(" = ");
        msg.append(String.format("%.3f",Math.toDegrees(phi[index])));
        msg.append(AMath.DEGREES);
        msg.append("  (" + String.format("%.3f",phi[index]) + " rad)");

        
        if (isEM != null)
        {
            msg.append("\n isEM = ");
            msg.append(isEM[index]);
        }
        if (label != null)
        {
            msg.append("\n label = ");
            msg.append(label[index]);
        }
        if (eOverp != null)
        {
            msg.append("\n eOverp = ");
            msg.append(eOverp[index]);
        }
        if (hasTrack != null)
        {
            msg.append("\n hasTrack = ");
            msg.append(hasTrack[index]);
        }

        return msg.toString();
    }

}

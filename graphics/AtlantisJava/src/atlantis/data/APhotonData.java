package atlantis.data;

import java.util.Vector;

import atlantis.event.*;
import atlantis.globals.AGlobals;
import atlantis.utils.AHashMap;
import atlantis.utils.AMath;
import atlantis.utils.A4Vector;

/**
 * Reconstructed Photon
 */
public class APhotonData extends AAODData
{
	private enum isEMe { Tight, Loose, none }
	
    private int[] isEM;
	
    private Vector<Enum> isEMEnum = new Vector<Enum>();
    private String[] label;

    APhotonData(AHashMap p, AEvent e)
    {
        super(p,e);
        String[] isEMString=p.getUnsureStringArray("isEMString");
        label=p.getUnsureStringArray("label");
        isEM=p.getUnsureIntArray("isEM");
       
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
        return "Photon";
    }

    public String getName()
    {
        return "Photon";
    }

    public String getisEMEnum(int index)
    {
    	return isEMEnum.get(index).toString();
    }
    
    public int getisEM(int index)
    {
        return isEM[index];
    }
    
    public String getlabel(int index)
    {
        return label[index];
    }

    public A4Vector get4Vector(int num, int[] list)
    {
        A4Vector sum = new A4Vector();
        for (int i = 0; i < num; ++i)
        {
            int k = list[i];
            A4Vector start = new A4Vector();
            start.setPtEtaPhiM(pT[k],eta[k],phi[k],0);
            sum.add(start);
        }
        return sum;
    }

    protected void applyCuts()
    {
        super.applyCuts();
        cut("CutsObjects", "PhotonPt", " |ET|", pT);
        
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
        
        cut("CutsObjects", "PhotonisEM", " isEM", isEM);

        if (parameterStore.get("CutsObjects", "PhotonisEMString").getStatus())
        {
        	int cutSub = parameterStore.get("CutsObjects", "PhotonisEMString").getI();
        	cutArrayEnum(isEMEnum, cutSub, "Photon isEMString");
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

        return msg.toString();
    }

}


package atlantis.data;

import atlantis.event.*;
import atlantis.globals.AGlobals;
import atlantis.utils.AHashMap;
import atlantis.utils.AMath;

/**
 * Reconstructed CompositeParticle
 */
public class ACompositeParticleData extends AAODData
{
    private int[] pdgId;
    private float[] charge;
    private String[] label;
    private String[] typeEV;

    ACompositeParticleData(AHashMap p, AEvent e)
    {
        super(p,e);
        pdgId = p.getUnsureIntArray("pdgId");
        charge=p.getUnsureFloatArray("charge");
        label=p.getStringArray("label");
        typeEV=p.getStringArray("typeEV");
    }

    public String getParameterGroup()
    {
        return "CompositeParticle";
    }

    @Override
    public String getName()
    {
        return "CompositeParticle";
    }

    public int getPdgId(int index)
    {
        return pdgId[index];
    }

    public float getCharge(int index)
    {
        return charge[index];
    }

    public String getLabel(int index)
    {
        return label[index];
    }

    public String getTypeEV(int index)
    {
        return typeEV[index];
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
        msg.append(" = ");
        msg.append(id[index]);
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

        if (charge != null)
        {
            msg.append("\n Charge = ");
            msg.append(charge[index]);
        }
        if(pdgId != null)
        {
            if(pdgId[index]!=0)
                msg.append("\n Type = " + APDGTable.getName(pdgId[index]) + " (type code " + pdgId[index] + ")" );
            else
                msg.append("\n Type = unknown (type code " + pdgId[index] + ")" );
        }
        msg.append("\n TypeEV = " + typeEV[index]);
        if (label != null)
            msg.append("\n Label = " + label[index]);
        return msg.toString();
    }

}

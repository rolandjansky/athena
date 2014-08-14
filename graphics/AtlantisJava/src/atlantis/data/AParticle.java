package atlantis.data;


import atlantis.output.AExceptionHandler;

import org.w3c.dom.*;


/**
 * This class is used to store the properties of a particle. Used by APDGTable.
 */
public class AParticle {
  private String name;
  private int charge;
  private int type;
  private int code;

  private String[] chargeRep=new String[] { "-", "0", "+"};

  public AParticle(Node node) {
    NamedNodeMap atts=node.getAttributes();

    try {
      charge=Integer.parseInt(atts.getNamedItem("charge").getNodeValue());
      type=Integer.parseInt(atts.getNamedItem("type").getNodeValue());
      code=Integer.parseInt(atts.getNamedItem("code").getNodeValue());
      name=atts.getNamedItem("name").getNodeValue();
    } catch(NumberFormatException e) {
      AExceptionHandler.processException("PDG error in Particle Node: "+node, e);
    }
  }

  public int getCharge() {
    return charge;
  }

  public int getType() {
    return type;
  }

  public int getCode() {
    return code;
  }

  public String getName() {
    return name;
  }

  public String getName(int sign) {
    if(charge!=0) {
      if(name.equals("Proton")&&sign==1) return "Proton";
      if(name.equals("Proton")&&sign==-1) return "Antiproton";
      return name+chargeRep[charge*sign+1];
    } else
      return name;
  }
}

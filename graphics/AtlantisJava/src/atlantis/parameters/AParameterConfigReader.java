package atlantis.parameters;

import org.w3c.dom.NamedNodeMap;
import org.w3c.dom.Node;

import atlantis.utils.xml.AXMLUtils;

/**
 * Construct AParameter objects from the XML configuration file.
 * 
 * @author waugh
 *
 */
public class AParameterConfigReader {
    /**
     * This function reads any type of parameters.
     * @param parameter The Node containing the definition of the parameter.
     * @return An AParameter object.
     */
    static AParameter readParameter(Node parameter, int groupScope) 
    {
        AParameter par=null;
        
        // the node name of the parameter decides its type
        String type=parameter.getNodeName();
        
        // read all the parameter atributes
        NamedNodeMap attributes=parameter.getAttributes();
        String fn=AXMLUtils.tryAttribut(attributes, "fn");
        String sn=AXMLUtils.tryAttribut(attributes, "sn");
        String sst=AXMLUtils.tryAttribut(attributes, "st");
        String sisMod=AXMLUtils.tryAttribut(attributes, "isMod");
        String sva=AXMLUtils.tryAttribut(attributes, "va");
        String op=AXMLUtils.tryAttribut(attributes, "op");
        String dop=AXMLUtils.tryAttribut(attributes, "dop");
        String to=AXMLUtils.tryAttribut(attributes, "to");
        String pv=AXMLUtils.tryAttribut(attributes, "pv");
        String tip=AXMLUtils.tryAttribut(attributes, "tip");
        String scopeAtt=AXMLUtils.tryAttribut(attributes, "scope");
        String unitsOfAtt=AXMLUtils.tryAttribut(attributes, "u");
        
        // set scope value for this parameter
        int scope=groupScope;
        if(scopeAtt!=null) 
        {
            if(scopeAtt.equals("LOCAL"))
                scope=AParameter.LOCAL;
            else if(scopeAtt.equals("GLOBAL"))
                scope=AParameter.GLOBAL;
        }
        
        // set user level for this parameter
        int ul=-1;
        if(!type.equals("LINK"))
            ul=Integer.parseInt(AXMLUtils.tryAttribut(attributes, "ul"));
        
        double va=0;
        if(sva!=null) va=Double.parseDouble(sva);
        
        // set initial status of this parameter
        boolean st=false;
        if(sst!=null)
            if(sst.equals("ON"))
                st=true;
        
        boolean isMod=false;
        if(sisMod!=null)
            if(sisMod.equals("YES"))
                isMod=true;
        
        int unitsOf=0;
        if(unitsOfAtt!=null)
            unitsOf=Integer.parseInt(unitsOfAtt);
        
        // create an AParameter object based on its type
        if(type.equals("LINK"))
            par=new ALinkParameter(to);
        else if(type.equals("COLOR"))
            par=new AColorParameter(fn, sn, tip, pv, va, false, st, ul, scope);
        else if(type.equals("SCOLOR"))
            par=new AColorParameter(fn, sn, tip, pv, va, true, st, ul, scope);
        else if(type.equals("ENUM"))
            par=new AEnumeratorParameter(fn, sn, tip, pv, va, false, st, ul, scope);
        else if(type.equals("SENUM"))
            par=new AEnumeratorParameter(fn, sn, tip, pv, va, true, st, ul, scope);
        else if(type.equals("ICUT"))
            par=new ACutParameter(fn, sn, tip, pv, AParameter.INT, va, op, dop, false, st, isMod, ul, scope, unitsOf);
        else if(type.equals("SICUT"))
            par=new ACutParameter(fn, sn, tip, pv, AParameter.INT, va, op, dop, true, st, isMod, ul, scope, unitsOf);
        else if(type.equals("FCUT"))
            par=new ACutParameter(fn, sn, tip, pv, AParameter.FLOAT, va, op, dop, false, st, isMod, ul, scope, unitsOf);
        else if(type.equals("SFCUT"))
            par=new ACutParameter(fn, sn, tip, pv, AParameter.FLOAT, va, op, dop, true, st, isMod, ul, scope, unitsOf);
        else if(type.equals("FLOAT"))
            par=new ANumberParameter(fn, sn, tip, pv, AParameter.FLOAT, va, false, st, isMod, ul, scope, unitsOf);
        else if(type.equals("SFLOAT"))
            par=new ANumberParameter(fn, sn, tip, pv, AParameter.FLOAT, va, true, st, isMod, ul, scope, unitsOf);
        else if(type.equals("INT"))
            par=new ANumberParameter(fn, sn, tip, pv, AParameter.INT, (int)va, false, st, isMod, ul, scope, unitsOf);
        else if(type.equals("SINT"))
            par=new ANumberParameter(fn, sn, tip, pv, AParameter.INT, (int)va, true, st, isMod, ul, scope, unitsOf);
        else if(type.equals("LINT"))
            par=new AListIntegerParameter(fn, sn, tip, pv, (int)va, false, st, ul, scope);
        else if(type.equals("SLINT"))
            par=new AListIntegerParameter(fn, sn, tip, pv, (int)va, true, st, ul, scope);
        else if(type.equals("STATUS"))
            par=new AStatusParameter(fn, sn, tip, st, ul, scope);
        else if(type.equals("StatusRoot"))
            par = new AStatusRootParameter(parameter, fn, sn, tip, ul, scope);
        else if(type.equals("StatusGroup"))
            par = new AStatusGroupParameter(parameter, fn, sn, tip, ul, scope);
        return par;
    }
}

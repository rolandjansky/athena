package atlantis.parameters;

import atlantis.canvas.ACanvas;
import java.util.*;

/**
 * This class is used to store a set of parameters. Provides functions for
 * fast searching of a parameter by its name.
 */
public class AParametersGroup {
    public static final int NO_USE=0;
    public static final int TABLE=1;
    public static final int PROJECTION=2;
    public static final int BRAIN_TEST=3;
    
    private String name;
    private String screenName;
    private String toolTip;
    private int scope;
    private int guiUsage;
    private int userLevel;
    
    private Vector<AParameter> parametersVector = new Vector<AParameter>();
    private HashMap<String, AParameter> parametersMap = new HashMap<String, AParameter>();
    private Vector<ACommand> commandsVector = new Vector<ACommand>();
    
    public AParametersGroup(String name, String screenName, int guiUsage, 
            int scope, int userLevel, String toolTip) {
        
        this.name=name;
        
        if(!screenName.trim().equals(""))
            this.screenName=screenName;
        else
            this.screenName=name;
        
        this.guiUsage=guiUsage;
        this.scope=scope;
        this.userLevel=userLevel;
        this.toolTip=toolTip;
    }
    
    public void restoreDefaults() {
        Enumeration<AParameter> parameters = parametersVector.elements();
        while(parameters.hasMoreElements()) {
            AParameter p = parameters.nextElement();
            if(p instanceof AAbstractParameter)
                ((AAbstractParameter)p).restoreDefaults();
        }
    }
    
    
    private void addToMap(AParameter p) {
        if(parametersMap.containsKey(p.getName()))
            throw new Error("Duplicate definition of parameter full name "+p.getName());
        
        parametersMap.put(p.getName(), p);
    }
    
    public void add(AParameter p) {
        parametersVector.addElement(p);
        if(!(p instanceof ALinkParameter))
            addToMap(p);
    }
    
    public void add(ACommand c) {
        for(int i=0; i<commandsVector.size(); i++) {
            ACommand command = commandsVector.elementAt(i);
            if(command.getName().equals(c.getName()))
                throw new Error("Duplicate definition of command "+c.getName());
        }
        commandsVector.addElement(c);
    }
    
    public int getScope() {
        return scope;
    }
    
    public String getGroupName() {
        return name;
    }
    
    public String getScreenName() {
        return screenName;
    }
    
    public String getToolTip() {
        return toolTip;
    }
    
    public int getUIUsage() {
        return guiUsage;
    }
    
    public int getUserLevel() {
        return userLevel;
    }
    
    public ACommand[] getCommands() {
        ACommand[] commands=new ACommand[commandsVector.size()];
        
        for(int i=0; i<commands.length; i++)
            commands[i]=commandsVector.elementAt(i);
        
        return commands;
    }
    
    public Vector<AParameter> getParameters(int userLevel) {
        Vector<AParameter> v=new Vector<AParameter>(parametersVector.size());
        
        for(int i=0; i<parametersVector.size(); i++) {
            AParameter p = parametersVector.elementAt(i);
            if(p.getUserLevel()<=userLevel)
                v.addElement(p);
        }
        
        return v;
    }
    
    public AParameter getParameter(String name) {
        if(parametersMap.get(name)!=null)
            return parametersMap.get(name);
        // if under group "Data", should find the parameter in the whole tree
        else if(parametersMap.get("Status")!=null){
            return ((AStatusRootParameter)parametersMap.get("Status")).getParameter(name);
        }
        else
            return null;
    }
    
    /**
     * Copy parameters from one window to another.
     * @param sourceName name of source window
     * @param destName name of destination window
     */
    public void copyParameters(String sourceName, String destName) {
        int source=ACanvas.getCanvas().getWindow(sourceName).getIndex();
        int dest=ACanvas.getCanvas().getWindow(destName).getIndex();
        
        for(int i=0; i<parametersVector.size(); i++) {
            AParameter p = parametersVector.elementAt(i);
            if(p.getScope()==AParameter.LOCAL)
                p.copy(source, dest);
        }
    }
    
    public void resolveLinkParameters() {
        for(int i=0; i<parametersVector.size(); i++) {
            AParameter p = parametersVector.elementAt(i);
            if(p instanceof ALinkParameter) {
                ((ALinkParameter)p).solveLink();
                addToMap(p);
            }
        }
    }
    
}

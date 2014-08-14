package atlantis.parameters;

import java.util.ArrayList;

import java.util.HashSet;
import java.util.Hashtable;
import java.util.List;
import java.util.Set;
import java.util.Vector;

import org.w3c.dom.NamedNodeMap;
import org.w3c.dom.Node;
import org.w3c.dom.NodeList;

import atlantis.canvas.ACanvas;
import atlantis.canvas.AWindow;
import atlantis.data.AJetData;
import atlantis.event.AEvent;
import atlantis.event.AEventManager;
import atlantis.event.ANewEventListener;
import atlantis.utils.ALogger;
import atlantis.utils.xml.AXMLUtils;

/**
 * The Atlantis parameters store. Contains a list of AParametersGroup
 * groups of parameters.
 * 
 * The class keeps a reference to a singleton instance, but this must initially be created using
 * the method {@link #construct(Node)}. Attempting to access the instance before initialization will
 * cause an Error to be thrown.
 * 
 * Ideally the APar instance would be passed to the classes that need it when the application is
 * initalized, but currently the instance is fetched where it is needed, in many different parts of 
 * the code.
 */
public final class APar implements ANewEventListener, AParameterChangeListener
{
    private static ALogger logger = ALogger.getLogger(APar.class);

    private final List<AParameterSuperGroup> superGroups = new ArrayList<AParameterSuperGroup>();

    /** Index of the currently selected window */
    private int currentWindowIndex;
    /** Index of the previously selected window.
     *  Used for obtaining temporary access to parameters of some window!
     */
    private int previousIndex;

    /** Parameter change listeners: just the GUI for now. */
    private List<AParameterChangeListener> parameterChangeListeners = new ArrayList<AParameterChangeListener>();

    /** The user level of the program */
    private int userLevel;

    /** The singleton instance */
    private static APar instance;

    /**
     * Create and return the singleton instance.
     * @param parameters
     */
    public synchronized static APar construct(Node parameters) {
        instance = new APar();
        instance.initialize(parameters);
        return instance;
    }

    /**
     * Get the singleton instance.
     * @return
     */
    public synchronized static APar instance() {
        if (instance==null) throw new Error("Attempted to access APar instance before initialization.");
        return instance;
    }

    /**
     * For testing only: create instance without initializing.
     * @return
     */
    public static synchronized APar constructDummyForTesting() {
        instance = new APar();
        return instance;
    }

    /**
     * Constructs dummy singleton instance for testing.
     */
    private APar() {}

    /**
     * Constructs the singleton instance by reading the config file.
     */
    private void initialize(Node parameters) 
    {
        String ul=parameters.getAttributes().getNamedItem("userLevel").getNodeValue();
        userLevel=Integer.parseInt(ul);

        NodeList superGroupNodes=parameters.getChildNodes();
        for(int s=0; s<superGroupNodes.getLength(); s++) 
        {
            Node superGroupNode = superGroupNodes.item(s);
            if (superGroupNode.getNodeType() == Node.ELEMENT_NODE) 
            {
                // here we have a <SuperGroup> element
                AParameterSuperGroup superGroup = AParameterSuperGroup.readSuperGroup(superGroupNode,userLevel);
                superGroup.addParameterChangeListener(this);

                this.superGroups.add(superGroup);
            }
        }
        // Can't resolve link parameters until all parameters configured.
        for (AParameterSuperGroup superGroup : superGroups) {
            superGroup.resolveLinkParameters();
        }

        previousIndex=-1;
    }

    /** Used to take into account differences in local parameters. */
    public void update(Node differences) 
    {
        if(differences==null) return;
        NodeList children=differences.getChildNodes();
        int childrenCount=children.getLength();
        for(int i=0; i<childrenCount; i++) 
        {
            Node diff=children.item(i);
            if(diff.getNodeType()==Node.ELEMENT_NODE) 
            {
                NamedNodeMap attributes=diff.getAttributes();
                String group=AXMLUtils.tryAttribut(attributes, "group");
                String name=AXMLUtils.tryAttribut(attributes, "name");
                String windowName=AXMLUtils.tryAttribut(attributes, "windowName");
                String va=AXMLUtils.tryAttribut(attributes, "va");
                String st=AXMLUtils.tryAttribut(attributes, "st");
                String op=AXMLUtils.tryAttribut(attributes, "op");
                AParameter p=get(group, name);
                if(p instanceof AAbstractParameter) 
                {
                    // When setting a global variable for just one window, make it local first
                    if (((AAbstractParameter)p).getScope() == AAbstractParameter.GLOBAL && !windowName.equals("*")) {
                        System.out.println("Setting scope to local for "+p.getName());
                        ((AAbstractParameter)p).setScope(AAbstractParameter.LOCAL);
                    }
                    if(va!=null&&!va.equals("")) 
                    {
                        ((AAbstractParameter)p)._setValue(windowName, Double.parseDouble(va));
                    }
                    if(st!=null&&!st.equals("")) 
                    {
                        if(st.equals("ON"))
                            ((AAbstractParameter)p)._setStatus(windowName, true);
                        else if(st.equals("OFF"))
                            ((AAbstractParameter)p)._setStatus(windowName, false);
                    }
                    if(op!=null&&!op.equals("")) 
                    {
                        ((AAbstractParameter)p)._setOperator(windowName, op);
                    }
                    ((AAbstractParameter)p).saveDefaults();
                }
            }
        }
    }

    /** Restore default settings of all parameters. */
    public void restoreDefaults() 
    {
        for (AParameterSuperGroup superGroup : superGroups) {
            superGroup.restoreDefaults();
        }
    }

    /**
     * Get parameter supergroups corresponding to current user level.
     * i.e. Those with a user level less than or equal to the current level.
     * @return supergroups corresponding to current user level
     */
    public List<AParameterSuperGroup> getUISuperGroups() {
        List<AParameterSuperGroup> uiSuperGroups = new ArrayList<AParameterSuperGroup>();
        for (AParameterSuperGroup superGroup : superGroups) {
            if (superGroup.getUserLevel()<=this.userLevel) uiSuperGroups.add(superGroup);
        }
        return uiSuperGroups;
    }

    /**
     * Get all the parameter groups used by the GUI, according to the user level.
     * @return the parameter groups
     */
    public AParametersGroup[][] getUIGroups() 
    {
        List<AParametersGroup[]> uiSuperGroups = new ArrayList<AParametersGroup[]>();
        for (AParameterSuperGroup superGroup : superGroups) {
            List<AParametersGroup> uiGroups = superGroup.getUIGroups();
            AParametersGroup[] uiGroupsArray = uiGroups.toArray(new AParametersGroup[uiGroups.size()]);
            uiSuperGroups.add(uiGroupsArray);
        }
        AParametersGroup[][] groups = 
                uiSuperGroups.toArray(new AParametersGroup[uiSuperGroups.size()][]);
        return groups;
    }

    /**
     * Get names of groups of given usage type to display in GUI.
     * @param uiUsage UI usage type to select
     * @return the group names
     */
    public Vector<String> getUIGroupNames(int uiUsage) {return getUIGroupNames(false,uiUsage);}

    /**
     * Get names of groups to display in GUI.
     * @return the group names
     */
    public Vector<String> getUIGroupNames() {return getUIGroupNames(true,0);}

    /**
     * Get names of groups to display in GUI.
     * @param selectAll true to select all regardless of UI usage
     * @param uiUsage ignored if selectAll is true
     * @return the group names
     */
    private Vector<String> getUIGroupNames(boolean selectAll, int uiUsage) {
        Vector<String> v=new Vector<String>();
        AParametersGroup[][] g=getUIGroups();
        for(int i=0;i<g.length;++i)
            for(int j=0;j<g[i].length;++j)
                if(selectAll || (g[i][j].getUIUsage()==uiUsage))
                    v.addElement(g[i][j].getGroupName());
        return v;
    }

    /**
     * Find parameter group with given name.
     * @param groupName name to look for
     * @return the group, or null if not found
     */
    public AParametersGroup getGroup(String groupName) 
    {
        for (AParameterSuperGroup superGroup : superGroups) {
            AParametersGroup group = superGroup.getGroup(groupName);
            if (group!=null) return group;
        }
        return null;
    }

    public int getUserLevel() 
    {
        return userLevel;
    }

    public AParameter get(String group, String name) 
    {
        AParametersGroup g = getGroup(group);
        if(g == null)
        {
            logger.debug("No group named \"" + group + "\"");
            return null;
        }

        AParameter p = g.getParameter(name);
        if(p == null)
        {
            logger.debug("No parameter named \"" + group + ":" + name + "\"");
        }

        return p;
    }

    public AParameter getUnknown(String group, String name) 
    {
        AParametersGroup grp = getGroup(group);
        if(grp==null)
            return null;
        else
            return grp.getParameter(name);
    }

    /**
     * Returns the values of an array of parameters
     * (can be ANY order: C1, C2, C3 ...)
     */
    public int[] getArray(String groupName, String name, int numEntries) 
    {
        int[] array=new int[numEntries];

        String baseName=name.substring(0, name.length()-1);
        int baseNumber=Integer.parseInt(name.substring(name.length()-1, name.length()));

        for(int i=0; i<numEntries; i++)
            array[i]=this.get(groupName, baseName+(baseNumber+i)).getI();

        return array;
    }

    boolean processParameterChangeCommand(String parameterChangeCommand) 
    {
        String parameterName, sValue;
        double value;
        AParameter parameter=null;
        String groupName = ACanvas.getCanvas().getCurrentWindow().getGroupName();

        for(int i=0; i<AParameter.supportedOperators.length; i++) 
        {
            int index=parameterChangeCommand.indexOf(AParameter.supportedOperators[i]);

            if(index!=-1) 
            {
                parameterName=parameterChangeCommand.substring(0, index).trim();
                AParametersGroup pg = getGroup(groupName);

                parameter=pg.getParameter(parameterName);

                /* AEvent calls listbox initialize in order to change its values from
                 event to event basis
                 AEnumeratorParameter.initialize() ->
                 ACommandProcessor.receive() -> .process() ->
                 APar.processParameterChangeCommand()

                 In case of ETMis - warning 'ETMisCollections=0.0 bad command' when
                 ATLAS->ETMis group wasn't the active one for a current window.
                 Following statement however continues if the group tab (ETMis) is
                 active, so it's not a bad command.
                 This will have to be dealt with when implementing the keyboard
                 commands mode of driving Atlantis, it would have to be possible to
                 change values, etc of group tabs which are not active. It should be
                 easy as local commands (like ETMisCollections) would say name of
                 parameter group they belong to. Proper groupName would the only problem.
                 */
                if(parameter==null)
                {
                    logger.debug("APar: warning: parameter " + parameterName +
                            " doesn't exist " + "in active group tab " + groupName);
                    return true; // prevent printing warning 'bad command ...'

                    // return false; - used to, hence raising 'bad command ... ' in
                    // ACommandProcessor.receive(). the same effect.
                }

                sValue=parameterChangeCommand.substring(index+AParameter.supportedOperators[i].length(), parameterChangeCommand.length()).trim();

                if(sValue.equals("")) break;

                try 
                {
                    value=Double.parseDouble(sValue);
                } 
                catch(NumberFormatException e) 
                {
                    return false;
                }

                return parameter.processCommand(AParameter.supportedOperators[i], value);
            }
        }

        AParametersGroup pg = getGroup(groupName);

        parameterName=parameterChangeCommand.trim();
        parameter=pg.getParameter(parameterName);
        if(parameter!=null) 
        {
            if(parameter.getStatus()==true)
                parameter.setStatus(false);
            else
                parameter.setStatus(true);
            return true;
        } 
        else
            return false;
    }

    /**
     * Copy parameters from one window to another.
     * @param sourceName name of source window
     * @param destName name of destination window
     */
    public void copyParameters(String sourceName, String destName) 
    {
        for (AParameterSuperGroup superGroup : superGroups) {
            superGroup.copyParameters(sourceName, destName);
        }
    }

    public void copyCurrentGroupLocalsTo(String destinationWindow) 
    {
        if(ACanvas.getCanvas().isValidWindowName(destinationWindow)) 
        {
            AWindow w=ACanvas.getCanvas().getCurrentWindow();
            Vector<AParameter> params=(getGroup(w.getGroupName())).getParameters(userLevel);

            int from=w.getIndex();
            int to=ACanvas.getCanvas().getWindow(destinationWindow).getIndex();

            for(int i=0; i<params.size(); i++) 
            {
                AParameter p=(AParameter)params.elementAt(i);

                if(p.getScope()==AParameter.LOCAL)
                    p.copy(from, to);
            }
        } 
        else
            throw new Error("copyCurrentGroupLocalsTo, unknown: "+destinationWindow);
    }

    /**
     * selectWindowParameters/restoreWindowParameters methods are used for
     * obtaining temporary access to parameters of some window
     */
    public void selectWindowParameters(String newWindow) 
    {
        //ACH - this was getting thrown sometimes, and I don't know why...
        //if(previousIndex!=-1)
        //throw new Error("selectWindowParameters, Try to select twice !?");

        if(ACanvas.getCanvas().isValidWindowName(newWindow)) 
        {
            previousIndex=getCurrentWindowIndex();
            setCurrentWindowIndex(ACanvas.getCanvas().getWindow(newWindow).getIndex());
        } 
        else 
            throw new Error("selectWindowParameters, window: "+newWindow+" , doesn't exist !?");
    }

    /**
     * selectWindowParameters/restoreWindowParameters methods are used for
     * obtaining temporary access to parameters of some window
     */
    public void restoreWindowParameters() 
    {
        if(previousIndex!=-1) 
        {
            setCurrentWindowIndex(previousIndex);
            previousIndex=-1;
        } 
        else
            throw new Error("restoreWindowParameters: you have nothing to restore !?");
    }

    /**
     * @param currentWindowIndex the currentWindowIndex to set
     */
    public void setCurrentWindowIndex(int currentWindowIndex) {
        this.currentWindowIndex = currentWindowIndex;
    }

    /**
     * @return the currentWindowIndex
     */
    public int getCurrentWindowIndex() {
        return currentWindowIndex;
    }

    /**
     * Add a listener to be informed of parameter changes.
     * 
     * @param listener
     */
    public void addParameterChangeListener(AParameterChangeListener listener) {
        parameterChangeListeners.add(listener);
    }

    /**
     * Tell the listeners that a parameter has changed.
     */
    @Override
    public void parameterChanged(AParameter parameter) {
        if(parameter.getName().equals("JetBTagger")){
            this.setDefaultBTaggerValue(parameter);
        }
        for (AParameterChangeListener listener : parameterChangeListeners) {
            listener.parameterChanged(parameter);
        }
    }

    @Override
    public void newEvent(AEvent event) {
        // update collection cut listboxes for collection-aware datatypes
        updateCollectionsListBoxes(event);
        updateBTaggerListBox(event);
    }

    /**
     * Update collection cut listboxes for collection-aware datatypes.
     */
    private void updateCollectionsListBoxes(AEvent event)
    {
        Hashtable<String,Vector<String>> collections = event.getCollections();
        String listBoxName = null;
        Hashtable<Integer, String> listBoxPreviousContents = null;
        Vector<String> keys = null;
        String[] keysArray = null;
        boolean contentsChanged = false; // contents of listbox change watchdog
        Set<AEnumeratorParameter> updatedEnumeratorParameters = new HashSet<AEnumeratorParameter>();

        // note - listBox.initialize()
        // if tab containing this particular listbox was active, it didn't
        // take effect to set different value by .setI() as default value
        // was in this case always the first one added - .initialize() called
        // automatically when tabpane is active
        // if tabpane wasn't active it worked fine
        for (String dataTypeName : collections.keySet())
        {
            contentsChanged = false;
            listBoxName = dataTypeName + "Collections";
            keys =  collections.get(dataTypeName);
            AEnumeratorParameter listBox = (AEnumeratorParameter) this.get(dataTypeName, listBoxName);
            keysArray = keys.toArray(new String[keys.size()]);

            listBoxPreviousContents = listBox.getValuesHashtable();

            for(int i = 0; i < listBoxPreviousContents.size(); i++)
            {
                String item = (String) listBoxPreviousContents.get(new Integer(i));
                if(keys.contains(item) || "All".equals(item))
                {
                    continue;
                }
                else
                {
                    contentsChanged = true;
                    listBox.removeItem(item);
                }
            }

            for(int i = 0; i < keysArray.length; i++)
            {
                if(listBoxPreviousContents.containsValue(keysArray[i]))
                {
                    continue;
                }
                else
                {
                    contentsChanged = true;
                    listBox.addItem(listBox.size(), keysArray[i]);
                }
            }

            // if the previous event had completely different collections than
            // the current one, 'all' item will be the only one which remained
            // after removal (the first loop). then the current (new) collections
            // were added in the secold loop and 'all' item was in this case the
            // fist, hence the default one - not desired
            // solution: if any changes were performed in these two loops
            // (contentsChanged variable set to true), the listbox will
            // get re-initialised anyway. by removing 'all' and subsequent
            // adding it the first collection from the event will be the default
            // one and 'all' would be the last item - as desired
            if(contentsChanged && !dataTypeName.endsWith("Segment"))
            {
                listBox.removeItem("All");
                listBox.addItem(listBox.size(), "All");
                listBox.initialize();
            }
            updatedEnumeratorParameters.add(listBox);
        } // while
        // Get the enumerator parameters so we can tell them about the new event.
        List<AEnumeratorParameter> collectionParameters = new ArrayList<AEnumeratorParameter>();
        for (AParameterSuperGroup superGroup : superGroups) {
            collectionParameters.addAll(superGroup.getCollectionParameters());
        }
        for (AEnumeratorParameter listBox : collectionParameters) {
            if (!updatedEnumeratorParameters.contains(listBox)) {
                listBox.clear();
                listBox.addItem(0, "None");
                listBox.initialize();
            }
        }
    } // updateCollectionsCutListBoxes() ------------------------------------

    /**
     * Update btagging cut listboxes for available btaggers.
     * 
     * @param event The current event
     * 
     * TODO: Make this independent of context (ie. pass it the name of the listBox to update, plus the values it should use) ? - Tom
     */
    private void updateBTaggerListBox(AEvent event)
    {
        AJetData currentjets = event.getJetData();
        if (currentjets==null) return;
        List<String> taggers = currentjets.getBTaggers();
        Set<AEnumeratorParameter> updatedEnumeratorParameters = new HashSet<AEnumeratorParameter>();
        AEnumeratorParameter listBox = (AEnumeratorParameter) this.get("CutsObjects", "JetBTagger");

        if(taggers.size() == 0) {
            listBox.clear();
            listBox.addItem(0, "None");
        }

        for(int i=0; i<taggers.size(); i++) {
            listBox.addItem(i, taggers.get(i));
        }

        listBox.initialize();
        updatedEnumeratorParameters.add(listBox);

    } // updateBTaggerListBox() ------------------------------------

    /**
     *  Keep a list of popular 'default' values for various b-taggers
     *  If we don't know about the b-tagger, use a fallback default of 1.0
     *  
     * @param parameter The current BTagger parameter
     */
    public void setDefaultBTaggerValue(AParameter parameter) {
        AEventManager eventManager = AEventManager.instance();
        
        //Necessary code to stop B-tagging listener when having no-jet-info xml file
        AJetData Jetss =   eventManager.getCurrentEvent().getJetData();
        if(Jetss == null)
            return;	
    
        // Grab the b-taggers from the current Jet Collection
        List<String> taggers = eventManager.getCurrentEvent().getJetData().getBTaggers();
        if(taggers.size() == 0)
            return;
    
        // Get the BTagName corresponding to the current selection (ordering!)
        //	String newTagger = taggers.get(parameter.getI());
        String newTagger = taggers.get(parameter.getI());
    
    
        if(newTagger.equals(new String("SV0"))){
            get("CutsObjects", "JetBTagweight").setD(5.25);
        }
        else if(newTagger.equals(new String("SV1"))){
            get("CutsObjects", "JetBTagweight").setD(1.00);
        }
        else if(newTagger.equals(new String("JetFitterCOMBNN"))){
            get("CutsObjects", "JetBTagweight").setD(2.4);
        }
        else if(newTagger.equals(new String("IP3D+SV1")) || newTagger.equals(new String("IP3DSV1"))){
            get("CutsObjects", "JetBTagweight").setD(1.55);
        }
        else if(newTagger.equals(new String("JetFitterTagNN"))){
            get("CutsObjects", "JetBTagweight").setD(1.55);
        }
        else if(newTagger.equals(new String("IP2D"))){
            get("CutsObjects", "JetBTagweight").setD(1.00);
        }
        else if(newTagger.equals(new String("IP3D"))){
            get("CutsObjects", "JetBTagweight").setD(1.00);
        }
        else if(newTagger.equals(new String("SV2"))){
            get("CutsObjects", "JetBTagweight").setD(1.00);
        }
        else if(newTagger.equals(new String("MV1"))){
            get("CutsObjects", "JetBTagweight").setD(0.601713);
        }
        else {
            get("CutsObjects", "JetBTagweight").setD(1.00);
        }
    }
}

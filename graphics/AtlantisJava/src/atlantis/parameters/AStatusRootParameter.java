package atlantis.parameters;

import hep.wired.util.TristateCheckBox;

import java.awt.Color;
import java.awt.Component;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;
import java.util.ArrayList;
import java.util.Iterator;

import javax.swing.JComponent;
import javax.swing.JPopupMenu;
import javax.swing.JTree;
import javax.swing.ToolTipManager;
import javax.swing.tree.DefaultTreeModel;
import javax.swing.tree.TreePath;

import org.w3c.dom.Node;
import org.w3c.dom.NodeList;

import atlantis.utils.AUtilities;
import atlantis.gui.ACheckBox;
import atlantis.gui.ACheckNode;
import atlantis.gui.AParametersTable;
import atlantis.gui.ATreeCellRenderer;

/**
 * AStatusRootParameter.java 
 * 
 * Created on 14 March 2005, 17:49 
 * 
 * @author Qiang Lu
 */
public class AStatusRootParameter extends AAbstractParameter
{
    private ArrayList<AParameter> parameterNodeList;

    private JTree statusTree;

    private ACheckNode statusRootNode;
    
    /** Creates a new instance of AStatusGroupParameter */
    AStatusRootParameter(Node statusRoot, String name, String screenName, String toolTip, int userLevel, int scope)
    {
        super(name, screenName, toolTip, AParameter.NO_VALUE, 0, null, null, false, false, 
                false, userLevel, scope, AAbstractParameter.NOTHING);
        // get child element of statusRoot
        NodeList childrens = statusRoot.getChildNodes();
        // report error if no parameters
        if (childrens == null)
            throw new Error("this group contains no parameters");
        int childrensCount = childrens.getLength();
        if (childrensCount == 0)
            throw new Error("this group contains no parameters");

        // create an AParameter object for each node under statusRoot
        // and add them into parameterNodeList
        parameterNodeList = new ArrayList<AParameter>();
        for (int i = 0; i < childrensCount; i++)
        {
            Node child = childrens.item(i);

            if (child.getNodeType() == Node.ELEMENT_NODE)
            {
                // here we have a <StatusGroup> or <Status> element
                parameterNodeList.add(AParameterConfigReader.readParameter(child, scope));
            }
        }
    }

    public void initialize()
    {
        super.initialize();

        // create the root node of the tree using the <StatusRoot> element
        TristateCheckBox rootCheckBox = new TristateCheckBox(screenName);
        rootCheckBox.setBackground(new Color(204, 204, 204));
        if (getScope() == LOCAL)
            rootCheckBox.setForeground(Color.black);
        else
            rootCheckBox.setForeground(Color.blue);

        rootCheckBox.setToolTipText(toolTip);
        statusRootNode = new ACheckNode(rootCheckBox, false, ACheckNode.MULTI_SELECTION, this);
        for (AParameter aNodeParameter : parameterNodeList)
        {
            aNodeParameter.initialize();
            if (aNodeParameter instanceof AStatusParameter)
            {
                if (aNodeParameter.getUserLevel() <= APar.instance().getUserLevel())
                {
                    ACheckBox statusCheckBox = ((AStatusParameter) aNodeParameter).getCheckBox();
                    ACheckNode aNode = new ACheckNode(statusCheckBox, statusCheckBox.isSelected(), ACheckNode.SINGLE_SELECTION, aNodeParameter);
                    statusRootNode.add(aNode);
                }
            }
            else if (aNodeParameter instanceof AStatusGroupParameter)
            {
                statusRootNode.add(((AStatusGroupParameter) aNodeParameter).getCheckNode());
            }
        }

        statusTree = new JTree(statusRootNode);
        statusTree.setBackground(new Color(204, 204, 204));
        statusTree.setCellRenderer(new ATreeCellRenderer());
        statusTree.addMouseListener(new NodeSelectionListener(statusTree));
        ToolTipManager.sharedInstance().registerComponent(statusTree);
    }

    public JComponent getValueComponent()
    {
        return null;
    }

    public JComponent getNameComponent()
    {
        return statusTree;
    }

    ArrayList<AParameter> getParameterList()
    {
        return parameterNodeList;
    }

    public AParameter getParameter(String name)
    {
        for (AParameter aNodeParameter : parameterNodeList)
        {
            if (aNodeParameter instanceof AStatusParameter)
            {
                if (aNodeParameter.getName().equals(name))
                    return aNodeParameter;
            }
            else if (((AStatusGroupParameter) aNodeParameter).getParameter(name) != null)
            {
                return ((AStatusGroupParameter) aNodeParameter).getParameter(name);
            }
        }
        return null;
    }

    public AParameter getGroupParameter(String name)
    {
        if(!isInitialized())
            initialize();
        
        if(getName().equals(name))
            return this;
        for (AParameter aNodeParameter : parameterNodeList)
        {
            if (aNodeParameter instanceof AStatusGroupParameter)
            {
                if (((AStatusGroupParameter) aNodeParameter).getGroupParameter(name) != null)
                {
                    return ((AStatusGroupParameter) aNodeParameter).getGroupParameter(name);
                }
            }
        }
        return null;
    }

    public boolean getStatus()
    {
        return statusRootNode.isSelected();
    }
    
    public void setStatus(boolean status)
    {
        for (AParameter aNodeParameter : parameterNodeList)
        {
            aNodeParameter.setStatus(status);
        }
    }
    
    public void refresh()
    {
        _refresh();
        fireParameterChanged();
    }

    public void setD(double v)
    {
        throw new IllegalArgumentException("StatusRootParameter hase no value");
    }

    public void setI(int v)
    {
        throw new IllegalArgumentException("StatusRootParameter hase no value");
    }

    public void saveChildrenDefaults()
    {
        for (AParameter aNodeParameter : parameterNodeList)
        {
            AAbstractParameter aNodeAbstractParameter = (AAbstractParameter) aNodeParameter;
            if(aNodeAbstractParameter instanceof AStatusGroupParameter)
                ((AStatusGroupParameter)aNodeAbstractParameter).saveChildrenDefaults();
            else
                ((AStatusParameter)aNodeAbstractParameter).saveDefaults();
        }
    }

    public void restoreDefaults()
    {
        for (AParameter aNodeParameter : parameterNodeList)
        {
            AAbstractParameter aNodeAbstractParameter = (AAbstractParameter) aNodeParameter;
            aNodeAbstractParameter.restoreDefaults();
        }
        if(statusTree != null)
        {
            for (int i = 1; i < statusTree.getRowCount(); i++)
                statusTree.collapseRow(i);
        }
    }

    public void copy(int from, int to) 
    {
        for (AParameter aNodeParameter : parameterNodeList)
        {
            AAbstractParameter aNodeAbstractParameter = (AAbstractParameter) aNodeParameter;
            aNodeAbstractParameter.copy(from, to);
        }
    }

    public String getDifferences(String groupName)
    {
        StringBuffer ret = new StringBuffer();
        
        for (AParameter aNodeParameter : parameterNodeList)
        {
            AAbstractParameter aNodeAbstractParameter = (AAbstractParameter) aNodeParameter;
            if(aNodeAbstractParameter instanceof AStatusGroupParameter)
                ret.append(((AStatusGroupParameter)aNodeAbstractParameter).getDifferences(groupName));
            else
                ret.append(((AStatusParameter)aNodeAbstractParameter).getDifferences(groupName));
        }
        
        return ret.toString();
    }
    
    public class NodeSelectionListener extends MouseAdapter
    {
        private JTree tree;

        private int offsetX;
        private int offsetY;

        NodeSelectionListener(JTree tree)
        {
            this.tree = tree;
            offsetX = 0;
            offsetY = 0;
        }

        public void setOffset(int x, int y)
        {
            offsetX = x;
            offsetY = y;
        }

        public int getOffsetX()
        {
            return offsetX;
        }

        public int getOffsetY()
        {
            return offsetY;
        }

        public void mousePressed(MouseEvent e)
        {
            if (AUtilities.isRightMouseButton(e))
            {
                AParameter clickedPar = null;
                int x = e.getX();
                int y = e.getY();
                int row = tree.getRowForLocation(x, y);
                TreePath path = tree.getPathForRow(row);
                if (path != null)
                {
                    Object clickedNode = path.getLastPathComponent();
                    if (clickedNode instanceof ACheckNode)
                    {
                        ACheckNode node = (ACheckNode) clickedNode;
                        clickedPar = node.getParameter();
                    }

                    final AParameter effectPar = clickedPar;
                    if(effectPar instanceof AStatusParameter)
                    {
                        JPopupMenu popupMenu = new JPopupMenu();
                        if (effectPar.getScope() == AParameter.LOCAL)
                        {
                            popupMenu.add(AParametersTable.SET_GLOBAL).addActionListener(new ActionListener()
                            {
                                public void actionPerformed(ActionEvent e)
                                {
                                    effectPar.changeScope(AParameter.GLOBAL);
                                    refresh();
                                }
                            });
                        }
                        else
                        {
                            popupMenu.add(AParametersTable.SET_LOCAL).addActionListener(new ActionListener()
                            {
                                public void actionPerformed(ActionEvent e)
                                {
                                    effectPar.changeScope(AParameter.LOCAL);
                                    refresh();
                                }
                            });
                        }
                        if (tree.isShowing())
                            popupMenu.show(tree, e.getX(), e.getY());
                        else
                            // when e.getSource() is the table
                            popupMenu.show((Component) e.getSource(), e.getX() + getOffsetX(), e.getY() + getOffsetY());
                    }
                    else
                    {
                        popupGroupScopeMenu(effectPar, e);
                    }
                }
            }
        }

        // groupPar is either AStatusRootParameter or AStatusGroupParameter
        private final void changeGroupScopes(AParameter groupPar, int scope)
        {
            Iterator<AParameter> it;
            if(groupPar instanceof AStatusRootParameter)
                it = ((AStatusRootParameter) groupPar).getParameterList().iterator();
            else
                it = ((AStatusGroupParameter) groupPar).getParameterList().iterator();
            while (it.hasNext())
            {
                AParameter aPar = it.next();
                // aPar is either AStatusGroupParameter or AStatusParameter
                if(aPar instanceof AStatusGroupParameter)
                    changeGroupScopes(aPar, scope);
                else
                    aPar.changeScope(scope);
            }
        }
        
        // groupPar is either AStatusRootParameter or AStatusGroupParameter
        private void popupGroupScopeMenu(AParameter groupPar, MouseEvent e)
        {
            final AParameter tmpPar = groupPar;
            JPopupMenu popupMenu = new JPopupMenu();
            popupMenu.add(AParametersTable.SET_ALL_LOCAL).addActionListener(
                    new ActionListener()
                    {
                        public void actionPerformed(ActionEvent e)
                        {
                            changeGroupScopes(tmpPar, AParameter.LOCAL);
                            refresh();
                        }
                    });
            popupMenu.add(AParametersTable.SET_ALL_GLOBAL).addActionListener(
                    new ActionListener()
                    {
                        public void actionPerformed(ActionEvent e)
                        {
                            changeGroupScopes(tmpPar, AParameter.GLOBAL);
                            refresh();
                        }
                    });
            if (tree.isShowing())
                popupMenu.show(tree, e.getX(), e.getY());
            else
                // when e.getSource() is the table
                popupMenu.show((Component) e.getSource(), e.getX() + getOffsetX(), e.getY() + getOffsetY());
        }
        
        public void mouseClicked(MouseEvent e)
        {
            if (AUtilities.isRightMouseButton(e))
            {
                return;
            }
            int x = e.getX();
            int y = e.getY();
            int row = tree.getRowForLocation(x, y);
            TreePath path = tree.getPathForRow(row);
            if (path != null)
            {
                Object clickedNode = path.getLastPathComponent();
                if (clickedNode instanceof ACheckNode)
                {
                    ACheckNode node = (ACheckNode) clickedNode;
                    boolean newSelectionFlag;
                    if (node.getParameter() instanceof AStatusParameter)
                        newSelectionFlag = !node.isSelected();
                    else
                    {
                        TristateCheckBox.State currentState = ((TristateCheckBox) node.getUserObject()).getState();
                        if (currentState == TristateCheckBox.NOT_SELECTED)
                            newSelectionFlag = true;
                        else
                            newSelectionFlag = false;
                    }
                    node.getClicked(newSelectionFlag, true);

                    if ((node.getSelectionMode() == ACheckNode.MULTI_SELECTION))
                    {
                        if (node.isSelected())
                        {
                            tree.expandPath(path);
                        }
                        else
                        {
                            if (row != 0)
                                tree.collapsePath(path);
                        }
                    }
                    ((DefaultTreeModel) tree.getModel()).nodeChanged(node);
                    // I need revalidate if node is root. but why?
                    if (row == 0)
                    {
                        tree.revalidate();
                        tree.repaint();
                    }
                }
            }
        }
    }

}

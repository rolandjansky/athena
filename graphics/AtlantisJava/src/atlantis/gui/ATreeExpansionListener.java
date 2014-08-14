/*
 * ATreeExpansionListener.java
 *
 * Created on 17 March 2005, 15:55
 */

package atlantis.gui;

import javax.swing.event.TreeExpansionListener;
import javax.swing.event.TreeExpansionEvent;
import javax.swing.JTree;

/**
 *
 * @author Qiang
 */
public class ATreeExpansionListener implements TreeExpansionListener {
    
    private JTree tree;
    
    private AParametersTable table;
    
    // the row number which the tree locates
    private int treeRow;
    
    /** Creates a new instance of ATreeExpansionListener */
    public ATreeExpansionListener(JTree tree, AParametersTable table, int treeRow) {
        this.tree = tree;
        this.table = table;
        this.treeRow = treeRow;
    }

    public void treeExpanded(TreeExpansionEvent event) {
        int newHeight = tree.getRowCount() * (int)(tree.getRowBounds(0).getHeight());
        table.setRowHeight(treeRow, newHeight);
    }

    public void treeCollapsed(TreeExpansionEvent event) {
        int newHeight = tree.getRowCount() * (int)(tree.getRowBounds(0).getHeight());
        table.setRowHeight(treeRow, newHeight);
    }
    
}

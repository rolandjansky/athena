package atlantis.gui;

import java.awt.Component;
import java.util.EventObject;
import java.util.Vector;

import javax.swing.JTable;
import javax.swing.JTree;
import javax.swing.event.CellEditorListener;
import javax.swing.table.AbstractTableModel;
import javax.swing.table.TableCellEditor;
import javax.swing.table.TableCellRenderer;

import atlantis.parameters.APar;
import atlantis.parameters.AParameter;
import atlantis.parameters.AParametersGroup;
import atlantis.parameters.AStatusRootParameter;

/**
 * The data model used to create the custom tables which contain parameters.
 */
class AParamGUIDataModel extends AbstractTableModel implements
        TableCellEditor, TableCellRenderer
{
    private Vector<AParameter> parametersList;
    // if there is a tree in a cell, the height of the cell should be decided by the tree
    private int[] rowHeight;
    // indicate if there is a tree in this table
    private boolean isTree = false;
    private JTree tree = null;
    private int treeRowNumber = -1;
    private ADefaultCellAttribute cellAtt;

    AParamGUIDataModel(AParametersGroup group)
    {
        parametersList = group.getParameters(APar.instance().getUserLevel());
        rowHeight = new int[parametersList.size()];

        cellAtt = new ADefaultCellAttribute(rowHeight.length, 2);

        for (int i = 0; i < parametersList.size(); i++)
        {
            AParameter p = (AParameter) parametersList.elementAt(i);
            if (!p.isInitialized())
                p.initialize();
            if (p instanceof AStatusRootParameter)
            {
                isTree = true;
                tree = (JTree) ((AStatusRootParameter) p).getNameComponent();
                treeRowNumber = i;
                rowHeight[i] = tree.getRowCount()
                        * (int) (tree.getRowBounds(0).getHeight());
                int[] columns = { 0, 1 };
                int[] rows = { i };
                cellAtt.combine(rows, columns);
            }
            else
                rowHeight[i] = 25;
        }
    }

    public ADefaultCellAttribute getCellAttribute()
    {
        return cellAtt;
    }

    int getRowHeight(int rowIndex)
    {
        return rowHeight[rowIndex];
    }

    public int getTreeRow()
    {
        return treeRowNumber;
    }

    public boolean hasTree()
    {
        return isTree;
    }

    public JTree getTree()
    {
        return tree;
    }

    public int getRowCount()
    {
        return parametersList.size();
    }

    public int getColumnCount()
    {
        return 2;
    }

    public Object getValueAt(int rowIndex, int columnIndex)
    {
        return new Integer(0);
    }

    public boolean isCellEditable(int rowIndex, int columnIndex)
    {
        return true;
    }

    public void refresh()
    {
        for (int i = 0; i < parametersList.size(); i++)
        {
            AParameter p = (AParameter) parametersList.elementAt(i);
            p.refresh();
        }
    }

    // TableCellRenderer implementation
    public Component getTableCellRendererComponent(JTable table, Object value,
            boolean isSelected, boolean hasFocus, int row, int column)
    {
        return getTableCellEditorComponent(table, value, isSelected, row,
                column);
    }

    // TableCellEditor implementation
    public Component getTableCellEditorComponent(JTable table, Object value,
            boolean isSelected, int row, int column)
    {
        AParameter p = (AParameter) parametersList.elementAt(row);
        switch (column)
        {
            case 0:
                return p.getNameComponent();
            case 1:
                return p.getValueComponent();
            default:
                return null;
        }
    }

    public AParameter getParameter(int row)
    {
        return (AParameter) parametersList.elementAt(row);
    }

    public Object getCellEditorValue()
    {
        return new Integer(0);
    }

    public boolean isCellEditable(EventObject anEvent)
    {
        return true;
    }

    public boolean shouldSelectCell(EventObject anEvent)
    {
        return true;
    }

    public boolean stopCellEditing()
    {
        return true;
    }

    public void cancelCellEditing()
    {}

    public void addCellEditorListener(CellEditorListener l)
    {}

    public void removeCellEditorListener(CellEditorListener l)
    {}
}

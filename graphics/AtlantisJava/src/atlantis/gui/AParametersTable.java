package atlantis.gui;

import java.awt.Dimension;
import java.awt.Rectangle;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;
import java.util.Enumeration;

import javax.swing.JPopupMenu;
import javax.swing.JTable;
import javax.swing.JTree;
import javax.swing.event.ListSelectionEvent;
import javax.swing.table.TableColumn;

import atlantis.parameters.AParameter;
import atlantis.parameters.AParametersGroup;
import atlantis.parameters.AStatusRootParameter;
import atlantis.utils.AUtilities;

/**
 * The graphical object which contains all the parameter of a single group.
 * Appears in the GUI.
 */
public class AParametersTable extends JTable
{
    private TableColumn column;
    private AParamGUIDataModel dataModel;
    public final static String SET_GLOBAL = "Set Global";
    public final static String SET_LOCAL = "Set Local";
    public final static String SET_ALL_GLOBAL = "Set All Global";
    public final static String SET_ALL_LOCAL = "Set All Local";
    
    public AParametersTable(AParametersGroup g)
    {
        super();
        dataModel = new AParamGUIDataModel(g);
        setModel(dataModel);
        for (int i = 0; i < dataModel.getRowCount(); i++)
            this.setRowHeight(i, dataModel.getRowHeight(i));
        if (dataModel.hasTree())
        {
            JTree theTree = dataModel.getTree();
            theTree.addTreeExpansionListener(new ATreeExpansionListener(
                    theTree, this, dataModel.getTreeRow()));
        }
        column = getColumnModel().getColumn(0);
        column.setHeaderValue("Name");
        column.setCellEditor(dataModel);
        column.setCellRenderer(dataModel);
        column.setPreferredWidth(110);
        column = getColumnModel().getColumn(1);
        column.setHeaderValue("Value");
        column.setCellEditor(dataModel);
        column.setCellRenderer(dataModel);
        column.setPreferredWidth(100);
        setUI(new AMultiSpanCellTableUI());
        setCellSelectionEnabled(true);
        
        addMouseListener(new MouseAdapter() {
            private int pressedX;
            private int pressedY;
            
            public void mousePressed(MouseEvent e)
            {
                int clickedRow = rowAtPoint(e.getPoint());
                int clickedCol = columnAtPoint(e.getPoint());
                final AParameter p = ((AParamGUIDataModel) getModel())
                .getParameter(clickedRow);
                if ((p instanceof AStatusRootParameter) && (clickedCol == 0))
                {
                    MouseListener[] theMLs = ((JTree) p.getNameComponent())
                    .getMouseListeners();
                    for (int i = 0; i < theMLs.length; i++)
                    {
                        if (theMLs[i] instanceof AStatusRootParameter.NodeSelectionListener)
                        {
                            Rectangle clickedRec = getCellRect(clickedRow,
                                    clickedCol, false);
                            ((AStatusRootParameter.NodeSelectionListener) theMLs[i])
                            .setOffset(clickedRec.x, clickedRec.y);
                            e.translatePoint(-clickedRec.x, -clickedRec.y);
                            if (AUtilities.isRightMouseButton(e))
                                theMLs[i].mousePressed(e);
                            else
                            {
                                pressedX = e.getX();
                                pressedY = e.getY();
                            }
                        }
                    }
                    return;
                }
                if (AUtilities.isRightMouseButton(e) && (clickedCol == 0))
                {
                    JPopupMenu popupMenu = new JPopupMenu();
                    if (p.getScope() == AParameter.LOCAL)
                    {
                        popupMenu.add(SET_GLOBAL).addActionListener(
                                new ActionListener() {
                                    public void actionPerformed(ActionEvent e)
                                    {
                                        p.changeScope(AParameter.GLOBAL);
                                        refresh();
                                    }
                                });
                    }
                    else
                    {
                        popupMenu.add(SET_LOCAL).addActionListener(
                                new ActionListener() {
                                    public void actionPerformed(ActionEvent e)
                                    {
                                        p.changeScope(AParameter.LOCAL);
                                        refresh();
                                    }
                                });
                    }
                    popupMenu.show(AParametersTable.this, e.getX(), e.getY());
                }
            }
            
            // have no idea why mouseClicked cannot be invoked when clicking the
            // table
            // only mousePressed & mouseReleased can be invoked
            public void mouseReleased(MouseEvent e)
            {
                int clickedRow = rowAtPoint(e.getPoint());
                // when the position of mouse releasing is out of the table,
                // clickedRow = -1
                // e.g. when you change a value of a combo box, and the list of
                // this combo box is rather long
                if (clickedRow < 0)
                    return;
                int clickedCol = columnAtPoint(e.getPoint());
                final AParameter p = ((AParamGUIDataModel) getModel())
                .getParameter(clickedRow);
                if ((p instanceof AStatusRootParameter) && (clickedCol == 0))
                {
                    MouseListener[] theMLs = ((JTree) p.getNameComponent())
                    .getMouseListeners();
                    for (int i = 0; i < theMLs.length; i++)
                    {
                        if (theMLs[i] instanceof AStatusRootParameter.NodeSelectionListener)
                        {
                            Rectangle clickedRec = getCellRect(clickedRow,
                                    clickedCol, false);
                            ((AStatusRootParameter.NodeSelectionListener) theMLs[i])
                            .setOffset(clickedRec.x, clickedRec.y);
                            e.translatePoint(-clickedRec.x, -clickedRec.y);
                            if ((pressedX == e.getX())
                                    && (pressedY == e.getY()))
                                theMLs[i].mouseClicked(e);
                        }
                    }
                    return;
                }
            }
        });
        // needed but I don't know why gary 25/10/2002
        refresh();
    }
    
    public void refresh()
    {
        dataModel.refresh();
        setEditingColumn(2);
        setEditingRow(0);
    }
    
    public Rectangle getCellRect(int row, int column, boolean includeSpacing)
    {
        Rectangle sRect = super.getCellRect(row, column, includeSpacing);
        if ((row < 0) || (column < 0) || (getRowCount() <= row)
                || (getColumnCount() <= column))
        {
            return sRect;
        }
        ADefaultCellAttribute cellAtt = ((AParamGUIDataModel) getModel()).getCellAttribute();
        if (!cellAtt.isVisible(row, column))
        {
            int temp_row = row;
            int temp_column = column;
            row += cellAtt.getSpan(temp_row, temp_column)[ADefaultCellAttribute.ROW];
            column += cellAtt.getSpan(temp_row, temp_column)[ADefaultCellAttribute.COLUMN];
        }
        int[] n = cellAtt.getSpan(row, column);
        
        int index = 0;
        int columnMargin = getColumnModel().getColumnMargin();
        Rectangle cellFrame = new Rectangle();
        int aCellHeight = sRect.height + rowMargin;
        cellFrame.y = sRect.y;
        cellFrame.height = n[ADefaultCellAttribute.ROW] * aCellHeight;
        
        Enumeration enumeration = getColumnModel().getColumns();
        while (enumeration.hasMoreElements())
        {
            TableColumn aColumn = (TableColumn) enumeration.nextElement();
            cellFrame.width = aColumn.getWidth() + columnMargin;
            if (index == column)
                break;
            cellFrame.x += cellFrame.width;
            index++;
        }
        for (int i = 0; i < n[ADefaultCellAttribute.COLUMN] - 1; i++)
        {
            TableColumn aColumn = (TableColumn) enumeration.nextElement();
            cellFrame.width += aColumn.getWidth() + columnMargin;
        }
        
        if (!includeSpacing)
        {
            Dimension spacing = getIntercellSpacing();
            cellFrame.setBounds(cellFrame.x + spacing.width / 2, cellFrame.y
                    + spacing.height / 2, cellFrame.width - spacing.width,
                    cellFrame.height - spacing.height);
        }
        return cellFrame;
    }
    
    public void columnSelectionChanged(ListSelectionEvent e)
    {
        repaint();
    }
    
    public void valueChanged(ListSelectionEvent e)
    {
        int firstIndex = e.getFirstIndex();
        int lastIndex = e.getLastIndex();
        if (firstIndex == -1 && lastIndex == -1)
        {
            repaint();
        }
        Rectangle dirtyRegion = getCellRect(firstIndex, 0, false);
        int numCoumns = getColumnCount();
        int index = firstIndex;
        for (int i = 0; i < numCoumns; i++)
        {
            dirtyRegion.add(getCellRect(index, i, false));
        }
        index = lastIndex;
        for (int i = 0; i < numCoumns; i++)
        {
            dirtyRegion.add(getCellRect(index, i, false));
        }
        repaint(dirtyRegion.x, dirtyRegion.y, dirtyRegion.width,
                dirtyRegion.height);
    }
    
}

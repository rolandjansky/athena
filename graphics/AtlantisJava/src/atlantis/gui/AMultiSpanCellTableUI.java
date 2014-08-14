package atlantis.gui;

import java.awt.Color;
import java.awt.Component;
import java.awt.Graphics;
import java.awt.Rectangle;

import javax.swing.JComponent;
import javax.swing.plaf.basic.BasicTableUI;
import javax.swing.table.TableCellRenderer;

public class AMultiSpanCellTableUI extends BasicTableUI
{
    public void paint(Graphics g, JComponent c)
    {
        for (int index = 0; index < table.getRowCount(); index++)
        {
            paintRow(g, index);
        }
    }
    
    private void paintRow(Graphics g, int row)
    {
        AParamGUIDataModel tableModel = (AParamGUIDataModel) table.getModel();
        ADefaultCellAttribute cellAtt = tableModel.getCellAttribute();
        int numColumns = table.getColumnCount();
        
        for (int column = 0; column < numColumns; column++)
        {
            Rectangle cellRect = table.getCellRect(row, column, true);
            int cellRow, cellColumn;
            if (cellAtt.isVisible(row, column))
            {
                cellRow = row;
                cellColumn = column;
            }
            else
            {
                cellRow = row + cellAtt.getSpan(row, column)[ADefaultCellAttribute.ROW];
                cellColumn = column + cellAtt.getSpan(row, column)[ADefaultCellAttribute.COLUMN];
            }
            paintCell(g, cellRect, cellRow, cellColumn);
        }
    }
    
    private void paintCell(Graphics g, Rectangle cellRect, int row, int column)
    {
        int spacingHeight = table.getRowMargin();
        int spacingWidth = table.getColumnModel().getColumnMargin();
        
        Color c = g.getColor();
        g.setColor(table.getGridColor());
        g.drawRect(cellRect.x, cellRect.y, cellRect.width - 1,
                cellRect.height - 1);
        g.setColor(c);
        
        cellRect.setBounds(cellRect.x + spacingWidth / 2, cellRect.y
                + spacingHeight / 2, cellRect.width - spacingWidth,
                cellRect.height - spacingHeight);
        
        if (table.isEditing() && table.getEditingRow() == row
                && table.getEditingColumn() == column)
        {
            Component component = table.getEditorComponent();
            component.setBounds(cellRect);
            component.validate();
        }
        else
        {
            TableCellRenderer renderer = table.getCellRenderer(row, column);
            Component component = table.prepareRenderer(renderer, row, column);
            
            if (component.getParent() == null)
            {
                rendererPane.add(component);
            }
            rendererPane.paintComponent(g, component, table, cellRect.x,
                    cellRect.y, cellRect.width, cellRect.height, true);
        }
    }
}

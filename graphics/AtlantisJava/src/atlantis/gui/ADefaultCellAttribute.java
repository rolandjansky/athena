package atlantis.gui;

import java.awt.Dimension;

public class ADefaultCellAttribute
{
    public static final int ROW    = 0;
    public static final int COLUMN = 1;
    
    //
    // !!!! CAUTION !!!!!
    // these values must be synchronized to Table data
    //
    protected int rowSize;
    protected int columnSize;
    protected int[][][] span;

    public ADefaultCellAttribute()
    {
        this(1, 1);
    }

    public ADefaultCellAttribute(int numRows, int numColumns)
    {
        setSize(new Dimension(numColumns, numRows));
    }

    protected void initValue()
    {
        for (int i = 0; i < span.length; i++)
        {
            for (int j = 0; j < span[i].length; j++)
            {
                span[i][j][COLUMN] = 1;
                span[i][j][ROW] = 1;
            }
        }
    }

    public int[] getSpan(int row, int column)
    {
        if (isOutOfBounds(row, column))
        {
            int[] ret_code = { 1, 1 };
            return ret_code;
        }
        return span[row][column];
    }

    public boolean isVisible(int row, int column)
    {
        if (isOutOfBounds(row, column))
            return false;
        if ((span[row][column][COLUMN] < 1)
                || (span[row][column][ROW] < 1))
            return false;
        return true;
    }

    public void combine(int[] rows, int[] columns)
    {
        if (isOutOfBounds(rows, columns))
            return;
        int rowSpan = rows.length;
        int columnSpan = columns.length;
        int startRow = rows[0];
        int startColumn = columns[0];
        for (int i = 0; i < rowSpan; i++)
        {
            for (int j = 0; j < columnSpan; j++)
            {
                if ((span[startRow + i][startColumn + j][COLUMN] != 1)
                        || (span[startRow + i][startColumn + j][ROW] != 1))
                {
                    return;
                }
            }
        }
        for (int i = 0, ii = 0; i < rowSpan; i++, ii--)
        {
            for (int j = 0, jj = 0; j < columnSpan; j++, jj--)
            {
                span[startRow + i][startColumn + j][COLUMN] = jj;
                span[startRow + i][startColumn + j][ROW] = ii;
            }
        }
        span[startRow][startColumn][COLUMN] = columnSpan;
        span[startRow][startColumn][ROW] = rowSpan;
    }

    public Dimension getSize()
    {
        return new Dimension(rowSize, columnSize);
    }

    public void setSize(Dimension size)
    {
        columnSize = size.width;
        rowSize = size.height;
        span = new int[rowSize][columnSize][2]; // 2: COLUMN,ROW
        initValue();
    }

    protected boolean isOutOfBounds(int row, int column)
    {
        if ((row < 0) || (rowSize <= row) || (column < 0)
                || (columnSize <= column))
        {
            return true;
        }
        return false;
    }

    protected boolean isOutOfBounds(int[] rows, int[] columns)
    {
        for (int i = 0; i < rows.length; i++)
        {
            if ((rows[i] < 0) || (rowSize <= rows[i]))
                return true;
        }
        for (int i = 0; i < columns.length; i++)
        {
            if ((columns[i] < 0) || (columnSize <= columns[i]))
                return true;
        }
        return false;
    }

}

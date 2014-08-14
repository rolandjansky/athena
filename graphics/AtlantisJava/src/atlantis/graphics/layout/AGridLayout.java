package atlantis.graphics.layout;

import java.awt.Component;
import java.awt.Container;
import java.awt.Dimension;
import java.awt.LayoutManager2;
import java.awt.Rectangle;

import javax.swing.JComponent;
import javax.swing.SwingUtilities;

import atlantis.canvas.ACanvas;

/**
 * This layout manager is a simplification of the
 * <code>ConstrainedGridLayout</code> layout manager written by Charles A.
 * Loomis. This layout manager hase two modes of operation: 1) CONSTRAINED -
 * cell size is calculated by dividing the container size acording to aspect
 * ratio. 2) PREFERRED - cell size is given at creation time
 * 
 * @author Dumitru Petrusca.
 */
public class AGridLayout implements LayoutManager2
{
    private Dimension gridSize;
    private int cellSize;
    private Dimension initialSize;

    public final static int CONSTRAINED = 0;
    public final static int PREFERRED = 1;
    private int layoutType;
    private boolean secondLayout;

    final public static String GRID_SIZE_ERROR = "Grid size must have width and height > 0.";

    final public static String ASPECT_RATIO_ERROR = "Aspect ratio must have width and height > 0.";

    final public static String CONSTRAINT_ERROR = "Constraint must be a Rectangle with positive width and height.";

    final public static String AGRID_LAYOUT_CONSTRAINT = "ConstrainedGridLayout.RectangularConstraint";

    /**
     * Creates a new GridLayout of type CONSTRAINNED.
     */
    public AGridLayout(Dimension gridSize, Dimension initialSize)
    {
        layoutType = CONSTRAINED;
        setGridSize(gridSize);
        secondLayout = true;
        this.initialSize = initialSize;
    }

    /**
     * Creates a new GridLayout of type PREFERRED
     */
    public AGridLayout(Dimension gridSize, int cellSize)
    {
        layoutType = PREFERRED;
        setGridSize(gridSize);
        this.cellSize = cellSize;
        secondLayout = false;
        initialSize = null;
    }

    private void setGridSize(Dimension gridSize)
    {
        // Check that all of the dimensions given are reasonable.
        if (gridSize.width <= 0 || gridSize.height <= 0 || gridSize.width > 100 || gridSize.height > 100)
            throw new IllegalArgumentException(GRID_SIZE_ERROR);

        // Copy the information into the layout manager.
        this.gridSize = new Dimension(0, 0);
        this.gridSize.setSize(gridSize);
    }

    public Dimension getGridSize()
    {
        return new Dimension(gridSize);
    }

    /**
     * Set (or reset) the constraints for the given component.
     */
    public void setConstraints(Component comp, Rectangle constraints)
    {
        if (comp instanceof JComponent)
        {
            Rectangle copy = new Rectangle(constraints);
            JComponent jc = (JComponent) comp;

            jc.putClientProperty(AGRID_LAYOUT_CONSTRAINT, copy);
        }
    }

    /**
     * Get the constraints being used for the given component.
     */
    public Rectangle getConstraints(Component comp)
    {
        Rectangle r = null;

        if (comp instanceof JComponent)
        {
            JComponent jc = (JComponent) comp;
            Object constraint = jc.getClientProperty(AGRID_LAYOUT_CONSTRAINT);

            if (constraint instanceof Rectangle)
                r = (Rectangle) constraint;
        }

        return (r != null) ? new Rectangle(r) : null;
    }

    // implementation of LayoutMamager interface

    public void addLayoutComponent(String name, Component comp)
    {}

    public void removeLayoutComponent(Component comp)
    {}

    public Dimension preferredLayoutSize(Container parent)
    {
        if (layoutType == CONSTRAINED)
        {
            Dimension d = parent.getSize();
            if (initialSize != null)
            {
                d = new Dimension(initialSize);
                initialSize = null;
            }

            int hCellSize = d.width / gridSize.width;
            int vCellSize = d.height / gridSize.height;
            if(ACanvas.getCanvas().getCurrentLayout().getName().equals("FULL SCREEN"))
            {
                return new Dimension(gridSize.width * hCellSize, gridSize.height * vCellSize);
            }
            cellSize = Math.min(hCellSize, vCellSize);
        }

        return new Dimension(gridSize.width * cellSize, gridSize.height * cellSize);
    }

    public Dimension minimumLayoutSize(Container parent)
    {
        return preferredLayoutSize(parent);
    }

    public void layoutContainer(Container parent)
    {
        Dimension d = parent.getSize();
        int hCellSize = d.width / gridSize.width;
        int vCellSize = d.height / gridSize.height;
        int compCount = parent.getComponentCount();

        for (int i = 0; i < compCount; i++)
        {
            Component comp = parent.getComponent(i);
            Rectangle r = getConstraints(comp);

            if (r != null)
            {
                if(ACanvas.getCanvas().getCurrentLayout().getName().equals("FULL SCREEN"))
                    comp.setBounds(r.x * hCellSize, r.y * vCellSize, r.width * hCellSize, r.height * vCellSize);
                else
                    comp.setBounds(r.x * cellSize, r.y * cellSize, r.width * cellSize, r.height * cellSize);
            }
        }

        if (secondLayout)
            SwingUtilities.invokeLater(new ARunAnotherLayout(parent));
    }

    // implementation of LayoutMamager2 interface

    /**
     * Adds the specified component to the layout, using the specified
     * constraint object.
     */
    public void addLayoutComponent(Component comp, Object constraints)
    {
        if (constraints instanceof Rectangle)
        {
            Rectangle r = (Rectangle) constraints;

            if (r.width <= 0 || r.height <= 0)
                throw new IllegalArgumentException(CONSTRAINT_ERROR);

            setConstraints(comp, r);
        }
        else if (constraints != null)
            throw new IllegalArgumentException(CONSTRAINT_ERROR);
    }

    public Dimension maximumLayoutSize(Container target)
    {
        return new Dimension(Integer.MAX_VALUE, Integer.MAX_VALUE);
    }

    public float getLayoutAlignmentX(Container target)
    {
        return 0.5f;
    }

    public float getLayoutAlignmentY(Container target)
    {
        return 0.5f;
    }

    public void invalidateLayout(Container target)
    {}

}

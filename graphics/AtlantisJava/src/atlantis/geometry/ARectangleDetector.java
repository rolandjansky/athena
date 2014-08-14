package atlantis.geometry;

import atlantis.graphics.ACoord;
import atlantis.utils.AMath;

public class ARectangleDetector extends ADetector
{
    double xMin;
    double xMax;
    double yMin;
    double yMax;

    ARectangleDetector(String name, String projection, String color, double xMin, double xMax, double yMin, double yMax)
    {
        super(name, projection, color);
        this.xMin = xMin;
        this.xMax = xMax;
        this.yMin = yMin;
        this.yMax = yMax;
    }

    protected ACoord getRZUser()
    {
        return getUser("RZ");
    }

    protected ACoord getFZUser()
    {
        return getUser("FZ");
    }

    protected ACoord getXZUser()
    {
        return getUser("XZ");
    }

    protected ACoord getYZUser()
    {
        return getUser("YZ");
    }

    protected ACoord getUser(String proj)
    {
        if (!projection.equals(proj))
            return ACoord.NO_DATA;
        return new ACoord(AMath.xBox((xMin + xMax) / 2., (xMax - xMin) / 2.), AMath.yBox((yMin + yMax) / 2., (yMax - yMin) / 2.));
    }

}

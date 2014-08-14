package atlantis.utils;

import java.util.HashMap;
import atlantis.Atlantis;

/**
 * Based on a standard Hashmap with an modified interface
 */

public class AHashMap extends HashMap
{

    private static ALogger logger = ALogger.getLogger(AHashMap.class);
    
    private void printWarningMessage(String msg)
    {
        logger.warn("AHashMap: " + msg);
    }


    public AHashMap(int num)
    {
        super(num);
    }

    public AHashMap put(String name, int i)
    {
        put(name, new Integer(i));
        return this;
    }

    public float[] getFloatArray(String name)
    {
        Object temp = get(name);
        if(temp == null)
        {
            printWarningMessage("getFloatArray(): can't find float array named " +
                                name);
            return null;
        }
        else
        {
            return (float[]) temp;
        }
    } // getFloatArray() -------------------------------------------------------

    public float[][][] getFloatArray3D(String name)
    {
        Object temp = get(name);

        if(temp == null)
        {
            printWarningMessage("getFloatArray3D(): can't find 3D float array named "
                                + name);
        }
        return (float[][][]) temp;
    }

    public String[] getStringArray(String name)
    {
        Object temp = get(name);

        if(temp == null)
        {
            printWarningMessage("getStringArray(): can't find String array named "
                                + name);
        }
        return (String[]) temp;
    }

    public String[][] getStringArray2D(String name)
    {
        Object temp = get(name);

        if(temp == null)
        {
            printWarningMessage("getStringArray2D(): can't find String array named "
                                + name);
        }
        return (String[][]) temp;
    }


    public int[] getIntArray(String name)
    {
        Object temp = get(name);

        if(temp == null && !name.equals("sub"))
        {
            printWarningMessage("getIntArray(): can't find int array named " +
                                name);
        }
        return (int[]) temp;
    }


    public int[][] getIntArray2D(String name)
    {
        Object temp = get(name);

        if(temp == null)
        {
            printWarningMessage("getIntArray2D(): can't find 2D int array named " +
                                name);
        }
        return (int[][]) temp;
    }


    public int getInt(String name)
    {
        Object temp = get(name);

        if(temp == null)
        {
            printWarningMessage("getInt(): can't find int named " + name);
        }
        return ((Integer) temp).intValue();
    }

    public long getLong(String name)
    {
        Object temp = get(name);

        if(temp == null)
        {
            printWarningMessage("getLong(): can't find int named " + name);
	    return -1;
        }
        return ((Long) temp).longValue();
    }

    public float getFloat(String name)
    {
        Object temp = get(name);

        if(temp == null)
        {
            printWarningMessage("getFloat(): can't find int named " + name);
	    return -1;
        }
        return ((Float) temp).floatValue();
    }


    public float[] getUnknownFloatArray(String name)
    {
        Object temp = get(name);

        if(temp == null)
            return new float[getInt("numData")];
        return (float[]) temp;
    }

    public long[] getUnknownLongArray(String name)
    {
        Object temp = get(name);

        if(temp == null)
            return new long[getInt("numData")];
        return (long[]) temp;
    }


    public int[] getUnknownIntArray(String name)
    {
        Object temp = get(name);

        if(temp == null)
            return new int[getInt("numData")];
        return (int[]) temp;
    }


    public float[] getUnsureFloatArray(String name)
    {
        Object temp = get(name);

        if(temp == null)
            return null;
        return (float[]) temp;
    }

    public int[] getUnsureIntArray(String name)
    {
        Object temp = get(name);

        if(temp == null)
            return null;
        return (int[]) temp;
    }

    public String[] getUnsureStringArray(String name)
    {
        Object temp = get(name);

        if(temp == null)
            return null;
        return (String[]) temp;
    }
}

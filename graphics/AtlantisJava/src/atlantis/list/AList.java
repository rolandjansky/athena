package atlantis.list;

import atlantis.event.AData;

/**
 * Instance of AList as seen in AListManager (Lists)
 * source references instance of the datatype in the list
 * <ul>
 *   <li>itemsIndex is a list of indices (Atlantis internal datatitems indices)
 *               of the particular datape which are in the list</li>
 *    <li>itemsID is a list of dataitems IDs of the datatype in the list</li>
 * </ul>
 *
 * <p>index and id is identical for Track for instance, but different for
 * calo cells. When masking data on cross-event basis, IDs are necessary
 * (e.g. masking hot calo cells based on their IDs)</p>
 *
 * <p>Copyright: Copyright (c) 2002</p>
 *
 * @author not attributable
 * @version 1.0
 */
public class AList
{
    private AData source;
    private int[] itemsIndex;
    private int[] itemsID;


    /**
     *
     * @param data AData
     * @param singleItem int - index of the dataitem to put in the list
     */
    public AList(AData data, int singleItem)
    {
        source = data;
        itemsIndex = new int[] { singleItem };
        itemsID = new int[] { source.getIdFromIndex(singleItem) };

    } // AList() ------------------------------------------------------------



    public AList(AData data, boolean[] drawn)
    {
        source = data;
        int num = 0;
        for(int i = 0; i < drawn.length; ++i)
        {
            if(drawn[i])
            {
                num++;
            }
        }

        itemsIndex = new int[num];
        itemsID = new int[num];
        num = 0;

        for(int i = 0; i < drawn.length; ++i)
        {
            if(drawn[i])
            {
                itemsIndex[num] = i;
                itemsID[num] = source.getIdFromIndex(i);
                num++;
            }
        }

    } // AList() ------------------------------------------------------------



    /**
     * String representation of the AList instance. Returns the datatype name,
     * resp. screen name and if there one item in the only, then the item's
     * ID. If there are more items, it says the number of dataitems.
     * @return String
     */
    public String toString()
    {
        String key = source.getStoreGateKey();
        String sn = source.getNameScreenName();
        String name = key != null ? sn + ":" + key : sn;

        if(itemsIndex.length == 1)
        {
            return name + " " + source.getIdFromIndex(itemsIndex[0]);
        }
        else
        {
            return name + " (" + itemsIndex.length + " items)";
        }

    } // toString() ---------------------------------------------------------



    public AData getSource()
    {
        return source;

    } // getSource() --------------------------------------------------------



    /**
     * Returns indices of the items in the list (internal Atlantis indices
     * which are *sometimes* identical with IDs)
     * @return int[]
     */
    public int[] getItems()
    {
        return itemsIndex;

    } // getItems() ---------------------------------------------------------



    public int[] getItemsID()
    {
        return itemsID;

    } // getItemsID() -------------------------------------------------------


} // class AList ============================================================

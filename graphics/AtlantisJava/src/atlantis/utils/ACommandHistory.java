package atlantis.utils;

import java.util.Vector;
import javax.swing.text.JTextComponent;


/**
 * Implementation of simple command history (up and down
 * arrows) and tab completion based upon the already typed commands.
 * No cross-sessions commands history so far.
 *
 * @author Zdenek Maxa
 */
public class ACommandHistory extends Vector
{
    // the component upon which the history operates
    private JTextComponent comp = null;
    // item on the size() position of the history (content of the text field)
    private String tempItem = "";
    private int pointer = 0;
    private int tabPointer = 0; // help pointer when doing tab completion
    private String tabItem = null;


    // ----------------------------------------------------------------------
    public ACommandHistory(JTextComponent compoment)
    {
        super();
        this.comp = compoment;
        this.pointer = 0;
        this.tabPointer = 0;
        this.tempItem = "";
        this.nullTabItem();

    } // ACommandHistory() --------------------------------------------------


    /**
     * add the item into the history container, if it's already there it's
     * removed first and then added (placed to the end)
     * @param item String
     */
    public void add(String item)
    {
        if(super.contains(item))
        {
            super.remove(item);
        }
        super.add(item);
        pointer = super.size();
        nullTabItem();

    } // add() --------------------------------------------------------------


    /**
     * erase the content of the tabItem string upon which the tab-completion
     * searching is performed
     */
    public void nullTabItem()
    {
        this.tabItem = null;

    } // nullTabItem() ------------------------------------------------------


    private String getItem()
    {
        if(pointer < super.size())
        {
            return super.get(pointer).toString();
        }
        else
        {
            return tempItem;
        }

    } // getItem() ----------------------------------------------------------



    private void desc()
    {
        if(pointer > 0)
        {
            pointer--;
        }

    } // desc() -------------------------------------------------------------



    private void inc()
    {
        if(pointer < super.size())
        {
            pointer++;
        }

    } // inc() --------------------------------------------------------------



    /**
     * compares the actual content of the text field with the history item
     * on the matching position. if they differ (user changed the item in the
     * text field) the item in the history (or the tempItem attribute - the
     * last item, not yet added to the container) gets updated
     */
    private void compareAndUpdate()
    {
        String currItem = comp.getText();
        if(pointer < super.size())
        {
            String histItem = super.get(pointer).toString();
            if(! histItem.equals(currItem))
            {
                super.remove(pointer);
                super.add(pointer, currItem);
            }
        }
        else
        {
            if(! tempItem.equals(currItem))
            {
                tempItem = currItem;
            }
        }

    } // compareAndUpdate() -------------------------------------------------



    /**
     * up-arrow handler - it will decrease the pointer and return former
     * command
     */
    public void upArrow()
    {
        if(! super.isEmpty())
        {
            compareAndUpdate();
            desc();
            comp.setText(getItem());
        }
        nullTabItem();

    } // upArrow() ----------------------------------------------------------



    /**
     * down-arrow handler - it will increase the pointer toward the more
     * recent commands
     */
    public void downArrow()
    {
        if(! super.isEmpty())
        {
            compareAndUpdate();
            inc();
            comp.setText(getItem());
        }
        nullTabItem();

    } // downArrow() --------------------------------------------------------



    /**
     * tabulator handler, tab-completion
     */
    public void tabulator()
    {
        // check tabItem - first or subsequent tab-key hit?
        if(tabItem == null)
        {
            tabItem = comp.getText().trim();
            if("".equals(tabItem) || tabItem == null)
            {
                nullTabItem();
                return;
            }
        }

        if(! super.isEmpty())
        {
            compareAndUpdate();

            if(tabPointer >= super.size())
            {
                tabPointer = 0; // rotate again from the beginning
            }

            for( ; tabPointer < super.size(); tabPointer++)
            {
                String item = super.get(tabPointer).toString();
                if(item.startsWith(tabItem))
                {
                    pointer = tabPointer;
                    comp.setText(item);
                    tabPointer++; // for next iteration (next tab hit)
                    break;
                }
            }

        } // if(! super.isEmpty())  [container empty]

    } // tabulator() --------------------------------------------------------



} // class ACommandHistory ==================================================

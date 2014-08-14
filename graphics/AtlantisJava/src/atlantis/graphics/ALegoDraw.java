package atlantis.graphics;
import java.awt.BasicStroke;

import java.awt.Color;
import java.awt.Font;
import java.awt.Stroke;
import java.awt.geom.Point2D;

import atlantis.canvas.AWindow;
import atlantis.event.AEvent;
import atlantis.data.ALVL1ResultData;
import atlantis.data.ATriggerInfoData;
import atlantis.graphics.colormap.AColorMap;
import atlantis.parameters.AEnumeratorParameter;
import atlantis.parameters.APar;
import atlantis.projection.AProjectionLegoPlot;
import atlantis.utils.AMath;

public class ALegoDraw
{

    protected static APar parameterStore = APar.instance();

    private static int legendHeight=0;
    private static int lineHeight = 17;//seperation of lines
    //colors to use in legend

    //width of lines in lego grid
    private static float legoGridWidth = 1.6f;
    private static float legoTowersWidth = 1.0f;

    private static Color textColor=Color.white, 
        backgroundColor=AColorMap.getColors()[parameterStore.get("Color", "BkgFill").getI()],
        borderColor=Color.lightGray;

    public static Color getBackgroundColor()
    {
        return backgroundColor;
    }

    public static void fillBackground(AWindow window, AGraphics ag)
    {

        backgroundColor = AColorMap.getColors()[parameterStore.get("Color", "BkgFill").getI()]; // inherit bkg colour from main background colour

        ag.setColor(backgroundColor);
        ag.fillRect(0, 0, window.getWidth(), window.getHeight());
    }

    public static void drawLegend(AWindow window, AGraphics ag, AEvent event, double met, double maxEt, double AODmaxEt)
    {	
        //colors to use
        textColor=Color.black;
        if(backgroundColor.getBlue()+backgroundColor.getGreen()+backgroundColor.getRed() < 400)
            textColor=Color.white;
        //change the font size (reset at end)
        float originalFontSize= ag.g.getFont().getSize2D();
        float newFontSize= (float) 0.9*originalFontSize;
        ag.g.setFont(ag.g.getFont().deriveFont(newFontSize));
        //variables to locate where to write/draw
        legendHeight = lineHeight+2;//distance from top of legend
        int indent = window.getWidth()- 200;//distance from left of legend
        int pos = 90;//distance to start writing second half of line
        //loop over Lvl1resultdata collections
        String[] colLvl1=event.getActiveCollectionNames("LVL1Result");
        ALVL1ResultData lvl1ResultData[] = null;
        if(colLvl1.length>0)
            lvl1ResultData = new ALVL1ResultData[colLvl1.length];
        for(int i=0;i<colLvl1.length;i++)
        {
            lvl1ResultData[i] = (ALVL1ResultData) event.get(colLvl1[i]);
        }
        ATriggerInfoData triggerInfoData = event.getTriggerInfoData();

        //check window is large enough
        if(window.getWidth()<250 || window.getHeight()<250)
        {
            drawWindowTooSmall(window, ag, indent);
            //reset font size
            ag.g.setFont(ag.g.getFont().deriveFont(originalFontSize));
            return;
        }

        //draw main legend
        if(parameterStore.get("LegoPlot", "MainLegend").getStatus())
            drawLegendMain(window, ag, event, met, maxEt, AODmaxEt, lvl1ResultData, indent, pos);
        //draw lvl1 sumEt and missEt
        if(parameterStore.get("LegoPlot", "L1EtLegend").getStatus())
            drawLegendL1ET(ag, lvl1ResultData, triggerInfoData, indent);
        //draw the item list
        boolean drawLegend=false;
        if(!drawLegend && parameterStore.get("LegoPlot", "L1Items").getStatus())drawLegend=true;
        if(!drawLegend && parameterStore.get("LegoPlot", "L2Items").getStatus())drawLegend=true;
        if(!drawLegend && parameterStore.get("LegoPlot", "EFItems").getStatus())drawLegend=true;
        if(drawLegend)
            drawLegendItems(window, ag, lvl1ResultData, indent);

        //reset font size
        ag.g.setFont(ag.g.getFont().deriveFont(originalFontSize));
    }

    public static void drawWindowTooSmall(AWindow window, AGraphics ag, int indent)
    {

        //change indent to make legend smaller
        indent+=135;
        // create legend boarder with two concentric rectangles
        // outer rectangle:
        ag.setColor(borderColor);
        ag.fillRect(indent - 4, 5, 65, 3*lineHeight);
        // inner rectangle:
        ag.setColor(backgroundColor);
        ag.fillRect(indent - 4 + 2, 7, 65 - 4, 3*lineHeight-4);
        ag.setColor(textColor);
        ag.updateColor();

        ag.drawString("Window", indent + 6, legendHeight);
        legendHeight += lineHeight;
        ag.drawString("too small", indent + 6, legendHeight);
        legendHeight += lineHeight;
        ag.drawString("for legend", indent + 6, legendHeight);
        legendHeight += lineHeight;
    }

    public static void drawLegendMain(AWindow window, AGraphics ag, AEvent event, double met, double maxEt, double AODmaxEt, ALVL1ResultData lvl1ResultData[], int indent, int pos)
    {

        // create legend boarder with two concentric rectangles
        // outer rectangle:
        ag.setColor(borderColor);
        //ag.fillRect(/*indent - 4*/window.getWidth()/1.8, /*5*/window.getHeight()/3.0, (int)(1-(window.getWidth()/1.8)), (int)(1-(window.getHeight()/3.0)));
        ag.fillRect(indent - 4, 5, 200, 105);

        // inner rectangle:
        ag.setColor(backgroundColor);
        ag.fillRect(indent - 4 + 2, 7, 200 - 4, 105 - 4);
        //ag.fillRect(/*indent - 4*/window.getWidth()/1.8 +2, /*5*/window.getHeight()/3.0 +2, (int)(1-(window.getWidth()/1.8))+4, (int)(1-(window.getHeight()/3.0))+4);

        ag.setColor(textColor);
        ag.updateColor();

        //First item Missing ET  (may remove met box from plot later)
        if(met!=0)
            drawLegendPartMain1(ag, event, met, indent, pos);
        else
            ag.drawString("No Missing Et", indent + 6, legendHeight);
        legendHeight += lineHeight;
        ag.setColor(textColor);
        ag.updateColor();

        //Second item colourby
        //make sure there are cells by checking the maximum energy
        //and that there have been colours added to the colourset
        //(can have maxE and no colours when scaling to AOD)
        if(maxEt!=0 && AProjectionLegoPlot.colorset.size()>0)
            drawLegendMain2(ag, indent, pos);
        else
        {
            ag.setColor(textColor);
            ag.updateColor();
            ag.drawString("No cells", indent + 6, legendHeight);
        }
        legendHeight += lineHeight;
        ag.setColor(textColor);
        ag.updateColor();

        //Third item maximum height of towers
        if(maxEt!=0 || AODmaxEt!=0)
            drawLegendMain3(ag, maxEt, AODmaxEt, indent, pos);
        else
        {
            ag.drawString("No towers", indent + 6, legendHeight);
            legendHeight += lineHeight;
        }
        legendHeight += lineHeight;
        ag.setColor(textColor);
        ag.updateColor();

        //Fourth item lvl1 result
        if (lvl1ResultData == null){
            ag.drawString("Trigger Decision N/A", indent+6, legendHeight);
            legendHeight+=lineHeight;//move by a line to keep same spacing
        }else if (lvl1ResultData.length>1){
            ag.drawString(">1 LVL1Result collections selected", indent+6, legendHeight);
            legendHeight+=lineHeight;
            ag.drawString("View on event properties or choose 1", indent+6, legendHeight);
        }else{
            drawLegendMain4(ag, lvl1ResultData[0], indent, pos);
        }
        legendHeight+=lineHeight;
        ag.setColor(textColor);
        ag.updateColor();
    }

    public static void drawLegendPartMain1(AGraphics ag, AEvent event, double met, int indent, int pos)
    {
        ag.setColor(AProjectionLegoPlot.defaultColorMap[parameterStore.get("ETMis", "Constant").getI()]);
        //draw dashed line
        ag.fillRect(indent + 6, 11, 5, 3); 
        ag.fillRect(indent + 6 + 10, 11, 5, 3);
        ag.fillRect(indent + 6 + 20, 11, 5, 3);
        ag.fillRect(indent + 6 + 30, 11, 5, 3);
        ag.setColor(textColor);
        ag.updateColor();
        String[] names = event.getActiveCollectionNames("ETMis");
        if(names.length==1)
            ag.drawString("Missing ET= " + Math.round(met) + " GeV" , indent + 0.6*pos, legendHeight);
        else
            ag.drawString("Max Missing ET= " + Math.round(met) + " GeV" , indent + 0.6*pos, legendHeight);
    }

    public static void drawLegendMain2(AGraphics ag, int indent, int pos)
    {
        String modeName;
        if (AProjectionLegoPlot.mode == 0)
        {
            modeName="LAr";
        }
        else if (AProjectionLegoPlot.mode == 1)
        {
            modeName="LVL1TriggerTower";
        }
        else
        {
            modeName="LVL1JetElement";
        }
        AEnumeratorParameter param = (AEnumeratorParameter) parameterStore.get(modeName, "ColorFunction");
        String colorFunction = param.getText(param.getI());
        ag.drawString(colorFunction + " (1-" + AProjectionLegoPlot.colorset.size() + ")",indent + pos, legendHeight);
        int barwidth = 6, barheight = 6;
        if(AProjectionLegoPlot.getDrawEMHAD())
        {
            if(AProjectionLegoPlot.colorset.size()==2)
            {
                //for Em/had option should only have 2 colors
                ag.setColor(AProjectionLegoPlot.defaultColorMap[AProjectionLegoPlot.colorEM]);
                ag.fillRect(indent + barwidth * 2, 28, barwidth * 2, barheight);
                ag.setColor(AProjectionLegoPlot.defaultColorMap[AProjectionLegoPlot.colorHad]);
                ag.fillRect(indent + 2 * barwidth* 2, 28, barwidth * 2, barheight);
            }
            else
            {
                //if more colors then lists must be in use
                int c = 0;
                ++c;
                ag.setColor(AProjectionLegoPlot.defaultColorMap[AProjectionLegoPlot.colorEM]);
                ag.fillRect(indent + c * barwidth, 28, barwidth, barheight);
                ++c;
                ag.setColor(AProjectionLegoPlot.defaultColorMap[AProjectionLegoPlot.colorHad]);
                ag.fillRect(indent + c * barwidth, 28, barwidth, barheight);
                for (int i = 0; i <AProjectionLegoPlot.caloColorMap.length ; i++)
                {
                    if (!AProjectionLegoPlot.colorset.contains(new Integer(i)))
                        continue;
                    if(i==AProjectionLegoPlot.colorEM || i==AProjectionLegoPlot.colorHad)
                        continue;
                    ++c;
                    ag.setColor(AProjectionLegoPlot.caloColorMap[i]);
                    ag.fillRect(indent + c * barwidth, 28, barwidth, barheight);
                }
            }
        }
        else
        {
            //if not Em/Had then loop over colors
            int c = 0;
            barwidth = 90/AProjectionLegoPlot.caloColorMap.length;
            for (int i = 0; i <AProjectionLegoPlot.caloColorMap.length ; i++)
            {
                if (!AProjectionLegoPlot.colorset.contains(new Integer(i)))
                    continue;
                ++c;
                ag.setColor(AProjectionLegoPlot.caloColorMap[i]);
                ag.fillRect(indent + c * barwidth, 28, barwidth, barheight);
            }
        }
    }

    public static void drawLegendMain3(AGraphics ag, double maxEt, double AODmaxEt, int indent, int pos)
    {
        ag.drawString( "Height of tallest tower:", indent + 6, legendHeight);
        //next line has the values
        legendHeight += lineHeight;
        if (parameterStore.get("LegoPlot", "ETAxisHeight").getD() > 0.0)
            ag.drawString("User selection: " + Math.round(maxEt) + " GeV ", indent +6, legendHeight);
        else if (parameterStore.get("LegoPlot", "ScaleToAOD").getStatus())
            ag.drawString("Scale to AOD: " + Math.round(maxEt) + " GeV ", indent +6, legendHeight);
        else
        {
            if(maxEt!=0)
            {
                ag.drawString("Cells: " + Math.round(maxEt) + " GeV ", indent +6, legendHeight);
                if(AODmaxEt!=0)
                    ag.drawString("AOD: " + Math.round(AODmaxEt) + " GeV ", indent + pos, legendHeight);
            }
            else
                ag.drawString("AOD: " + Math.round(AODmaxEt) + " GeV ", indent + 6, legendHeight);
        }   
    }

    public static void drawLegendMain4(AGraphics ag, ALVL1ResultData lvl1ResultData, int indent, int pos)
    {
        ag.drawString("Trigger Decision:", indent +6, legendHeight);
        legendHeight+=lineHeight;
        String[] trigRes={"NA","NA","NA"};
        int[] temp=new int[3];
        temp[0]=lvl1ResultData.getPassedL1(0);
        temp[1]=lvl1ResultData.getPassedL2(0);
        temp[2]=lvl1ResultData.getPassedEF(0);
        for(int i=0;i<3;i++)
        {
            switch(temp[i])
            {
            case -1:
                trigRes[i]="N/C";
                break;
            case 0:
                trigRes[i]="failed";
                break;
            case 1:
                trigRes[i]="passed";
                break;
            default:
                trigRes[i]="NA";
            }
        }
        ag.drawString("L1:" + trigRes[0] +" L2:" + trigRes[1]+" EF:" + trigRes[2], indent +6, legendHeight);
    }

    public static void drawLegendL1ET(AGraphics ag, ALVL1ResultData lvl1ResultData[], ATriggerInfoData triggerInfoData, int indent)
    {
        // create legend boarder with two concentric rectangles
        // outer rectangle:
        ag.setColor(borderColor);
        ag.fillRect(indent - 4, legendHeight, 200, (int) (lineHeight*1.5));
        // inner rectangle:
        ag.setColor(backgroundColor);
        ag.fillRect(indent - 4 + 2, legendHeight + 2, 200 - 4, (int) (lineHeight*1.5 - 4));
        ag.setColor(textColor);
        ag.updateColor();
        legendHeight+=lineHeight;
        //if no trigger info and "All" Lvlresult selected display warning
        if(triggerInfoData==null && lvl1ResultData!=null && lvl1ResultData.length>1){
            ag.drawString(">1 LVL1Result collections selected", indent+6, legendHeight);
            legendHeight+=lineHeight; 
            return;
        }

        float LVL1EtMiss=-1.0f,LVL1SumEt=-1.0f;
        if(triggerInfoData!=null)
        {
            LVL1EtMiss=triggerInfoData.getEnergyEtMiss(0);
            LVL1SumEt=triggerInfoData.getEnergySumEt(0);  
        }
        else if(lvl1ResultData!=null && lvl1ResultData.length==1)
        {
            LVL1EtMiss=lvl1ResultData[0].getEnergyEtMiss(0);
            LVL1SumEt=lvl1ResultData[0].getEnergySumEt(0);
        }
        String LVL1EtMissOutput,LVL1SumEtOutput;
        if(LVL1EtMiss>0) 
            LVL1EtMissOutput=""+LVL1EtMiss;
        else 
            LVL1EtMissOutput="N/A";
        if(LVL1SumEt>0) 
            LVL1SumEtOutput=""+LVL1SumEt;
        else 
            LVL1SumEtOutput="N/A";

        ag.drawString("L1-EtMiss: "+LVL1EtMissOutput+" L1-SumEt: "+LVL1SumEtOutput, indent , legendHeight);
        legendHeight+=lineHeight;
    }

    public static void drawLegendItems(AWindow window, AGraphics ag, ALVL1ResultData lvl1ResultData[], int indent)
    {
        //   ag.setStroke(new BasicStroke(legoTowersWidth)); returning to original line width

        if(lvl1ResultData!=null && lvl1ResultData.length>1){
            //create legend boarder with two concentric rectangles
            //outer rectangle:
            ag.setColor(borderColor);
            ag.fillRect(indent - 4, legendHeight, 200, (int) (lineHeight*5.5));
            // inner rectangle:
            ag.setColor(backgroundColor);
            ag.fillRect(indent - 4 + 2, legendHeight + 2, 200 - 4, (int) (lineHeight*5.5 - 4));
            ag.setColor(textColor);
            ag.updateColor();
            legendHeight+=lineHeight;
            ag.drawString(">1 LVL1Result collections selected", indent+6, legendHeight);
            legendHeight+=lineHeight;
            ag.drawString("Either select 1 LVL1Result collection", indent+6, legendHeight);
            legendHeight+=lineHeight;
            ag.drawString("from: Calo->LVL1Result", indent+6, legendHeight);
            legendHeight+=lineHeight;
            ag.drawString("or to see all items select:", indent+6, legendHeight);
            legendHeight+=lineHeight;
            ag.drawString("file->Event Properties", indent+6, legendHeight);
            legendHeight+=lineHeight; 
            return;
        }

        //output message to say no lvl1result
        if(lvl1ResultData==null){
            ag.setColor(borderColor);
            ag.updateColor();
            ag.fillRect(window.getWidth() -200 - 4, legendHeight+4, 200, (int) Math.round(1.5*lineHeight));
            ag.setColor(backgroundColor);
            ag.updateColor();
            ag.fillRect(window.getWidth() -200 - 4 + 2, legendHeight +4+ 2, 200 - 4, (int) Math.round(1.5*lineHeight)-4);
            ag.setColor(textColor);
            ag.updateColor();
            legendHeight+=lineHeight;
            ag.drawString("No LVL1Result collection in event", window.getWidth() - 200 + 2, legendHeight+4);
            return;
        }

        int origlineHeight =lineHeight;//save lineheight to be reset at end of function
        lineHeight*=0.8;//for item list bunch together
        int startrowHeight =legendHeight;

        //now output items in columns in new legend box

        //lists to draw
        int dolist1 = parameterStore.get("LegoPlot", "L1Items").getStatus()?1:0;
        int dolist2 = parameterStore.get("LegoPlot", "L2Items").getStatus()?1:0;
        int dolist3 = parameterStore.get("LegoPlot", "EFItems").getStatus()?1:0;

        //number of items per column and number of columns
        int maxItems=(int) Math.round(((0.8*window.getHeight()-legendHeight)/lineHeight )-0.5);
        int columnWidth= 72;
        int maxColumns=(int) Math.floor((0.9*window.getWidth())/columnWidth);

        // store items and headers
        String[][] items=new String[3][];
        if(dolist1>0) items[0]=newList("---L1 items---", lvl1ResultData[0].getCtpItemListSplit(0,true));
        if(dolist2>0) items[1]=newList("---L2 items---", lvl1ResultData[0].getitemListL2Split(0,true));
        if(dolist3>0) items[2]=newList("---EF items---", lvl1ResultData[0].getitemListEFSplit(0,true));

        //number of columns for each list 
        int[] num=new int[3];
        num[0]= (dolist1==0)? 0 : (int) Math.ceil(items[0].length/((double)maxItems));
        num[1]= (dolist2==0)? 0 : (int) Math.ceil(items[1].length/((double)maxItems));
        num[2]= (dolist3==0)? 0 : (int) Math.ceil(items[2].length/((double)maxItems));

        boolean doWarning=false;//for printing see event properties message
        //calculate total number of columns
        if(num[0]+num[1]+num[2]>maxColumns){
            //too many in list so each can just have the average
            int av = (int) Math.floor(maxColumns/((double)(dolist1+dolist2+dolist3)));
            if(dolist1>0) num[0]=av;
            if(dolist2>0) num[1]=av;
            if(dolist3>0) num[2]=av;
            doWarning=true;
        }
        int noOfColumns=num[0]+num[1]+num[2];

        //now store each column into an array [column][row]
        String[][] list=new String[noOfColumns][maxItems];
        int list_counter=0;
        //loop over each list
        for(int i=0; i<3; i++){
            //loop over items in a list
            int item_counter=0;
            //only loop over columns can display for each list
            for(int j=0; j<num[i]; j++){
                //loop over all items for a column
                for(int k=0; k<maxItems; k++){
                    if(item_counter<items[i].length)
                        list[list_counter][k]=items[i][item_counter];
                    else
                        list[list_counter][k]="";
                    item_counter++;
                }
                //next column
                list_counter++;
            }
        }

        // makes outer and inner borders of table:
        ag.setColor(borderColor);
        ag.fillRect(window.getWidth() -noOfColumns*columnWidth - 4, legendHeight, (int)(noOfColumns*columnWidth), (int) Math.round((maxItems+0.5)*lineHeight));

        //draw columns
        for(int i=0; i<noOfColumns; i++){
            //draw column to draw onto
            legendHeight=startrowHeight;
            ag.setColor(backgroundColor);
            ag.updateColor();
            ag.fillRect(window.getWidth() -(noOfColumns-i)*columnWidth - 4 + 2, legendHeight + 2, columnWidth - 4, (int) Math.round((maxItems+0.5)*lineHeight)-4);
            ag.setColor(textColor);
            ag.updateColor();
            //draw item
            for(int j=0; j<maxItems; j++){
                legendHeight+=lineHeight;   
                ag.drawString(list[i][j], window.getWidth() - (noOfColumns-i)*columnWidth + 2, legendHeight);
            }
        }

        //output warning if too many items
        if(doWarning){
            legendHeight+=lineHeight;   
            //draw under columns
            ag.setColor(borderColor);
            ag.updateColor();
            ag.fillRect(window.getWidth() -90 - 4, legendHeight+4, 90, (int) Math.round(2.5*lineHeight));
            ag.setColor(backgroundColor);
            ag.updateColor();
            ag.fillRect(window.getWidth() -90 - 4 + 2, legendHeight +4+ 2, 90 - 4, (int) Math.round(2.5*lineHeight)-4);
            ag.setColor(textColor);
            ag.updateColor();
            legendHeight+=lineHeight;
            ag.drawString(" more items in", window.getWidth() - 90 + 2, legendHeight+4);
            legendHeight+=lineHeight;
            ag.drawString("event properties", window.getWidth() - 90 + 2, legendHeight+4);
        }
        lineHeight=origlineHeight;//reset line height
    }

    static String[] newList(String first, String[] rest){
        int counter = 2;//1 for title 1 for N/A if needed
        if(rest!=null) counter+=rest.length-1;//now don't need N/A

        String[] editedList=new String[counter];
        editedList[0]=first;
        if(rest!=null){
            System.arraycopy(rest, 0, editedList, 1, rest.length);
        }else{
            editedList[1]="N/A";
        }

        return editedList;
    }

    static void drawline(AWindow window, AGraphics ag, Point2D.Double from, Point2D.Double to)
    {
        from=window.calculateDisplay(from);
        to=window.calculateDisplay(to);
        ag.drawLine( from.x,from.y,to.x,to.y);
    }

    static void drawline(AWindow window, AGraphics ag, double fromX, double fromY, double toX, double toY)
    {
        Point2D.Double from= new Point2D.Double(fromX,fromY);
        Point2D.Double to= new Point2D.Double(toX,toY);
        drawline(window, ag, from, to);
    }

    //converts from energy into eta coords
    static void drawline(AWindow window, AGraphics ag, double fromX, double fromY, double toX, double toY, int etAxisMaximum)
    {
        fromY=-5-(1-AProjectionLegoPlot.getyz(window.getIndex()))*50*(fromY/etAxisMaximum)*0.9;
        toY=-5-(1-AProjectionLegoPlot.getyz(window.getIndex()))*50*(toY/etAxisMaximum)*0.9;
        Point2D.Double from= new Point2D.Double(fromX,fromY);
        Point2D.Double to= new Point2D.Double(toX,toY);
        drawline(window, ag, from, to);
    }

    static void drawstring(AWindow window, AGraphics ag, String str, double hereX, double hereY)
    {
        Point2D.Double here= new Point2D.Double(hereX,hereY);
        here=window.calculateDisplay(here);
        ag.drawString(str, here.x,here.y);
    }

    //converts from energy into eta coords
    static void drawstring(AWindow window, AGraphics ag, String str, double hereX, double hereY, int etAxisMaximum)
    {
        hereY=-5-(1-AProjectionLegoPlot.getyz(window.getIndex()))*50*(hereY/etAxisMaximum)*0.9;  
        Point2D.Double here= new Point2D.Double(hereX,hereY);
        here=window.calculateDisplay(here);
        ag.drawString(str, here.x,here.y);
    }

    public static void drawGrid(AWindow window, AGraphics ag, double maxEt)
    {   	
        ag.setStroke(new BasicStroke(legoGridWidth));  //setting grid line width 
        int width = window.getSize().width;
        int dpi = 72;
        // Set a scaling font size with window width (sqrt prevents text from going too small in the small window)
        int newFontSize = (int) Math.round(35.0 * Math.sqrt(width) / (double) dpi);

        //change the font size (reset at end)
        float originalFontSize= ag.g.getFont().getSize2D();
        Font f = new Font("SansSerif", Font.PLAIN, newFontSize);        
        ag.setFont(f);

        // Draw the scale and labels on Et axis    
        ag.setColor(Color.darkGray);
        if(backgroundColor.getBlue()+backgroundColor.getGreen()+backgroundColor.getRed() < 400)
            ag.setColor(Color.lightGray);
        ag.updateColor();                        
        int etAxisMaximum=(int) maxEt;//convert to int for use later on
        int etAxisScale = 10;//seperation of ticks in ET
        if (AProjectionLegoPlot.defaultScale ==1 )
        {
            etAxisScale=1;
        }
        else if (maxEt > 10.0)
        {
            //set scale to 0.1 of nearest hundred to maximum
            etAxisScale = 10 * ((int) Math.ceil(maxEt / 100.0));
        }
        else if (maxEt == 10.0)
        {
            etAxisScale = 5;
        }
        else if (maxEt < 10.0)
        {
            etAxisScale = 2;
        }
        double lowPhi=-360*AProjectionLegoPlot.getxz(window.getIndex());
        //lowPhi = 1;
        int ticksizex = 10;
        double ticksizey = 0.5;		
        //Draw Et axis
        drawline(window,ag,lowPhi,0.0,lowPhi,etAxisMaximum, etAxisMaximum);
        //Draw the ticks
        for(int s = etAxisMaximum-etAxisScale; s >= 0.0; s-=etAxisScale)
        {                         
            drawline(window,ag,lowPhi, s,lowPhi + ticksizex,s, etAxisMaximum);
            if (AProjectionLegoPlot.defaultScale ==1 )
                drawstring(window,ag,"10^" + Integer.toString( s + AProjectionLegoPlot.minimumofLogScale ),lowPhi + 2*ticksizex,s,etAxisMaximum);
            else
                drawstring(window,ag,Integer.toString( s ),lowPhi + 2*ticksizex,s,etAxisMaximum);
        }
        // Draw the title on Et axis
        drawline(window,ag,lowPhi, etAxisMaximum,lowPhi + ticksizex,etAxisMaximum, etAxisMaximum);
        if (AProjectionLegoPlot.defaultScale ==1)
            drawstring(window,ag,"10^" + Integer.toString( etAxisMaximum + AProjectionLegoPlot.minimumofLogScale) + " ET (GeV)",lowPhi + 2*ticksizex,etAxisMaximum,etAxisMaximum);
        else if(AProjectionLegoPlot.defaultScale ==2)
            drawstring(window,ag,Integer.toString( etAxisMaximum ) + " ET (GeV)^1/2",lowPhi + 2*ticksizex,etAxisMaximum,etAxisMaximum);
        else
            drawstring(window,ag,Integer.toString( etAxisMaximum ) + " ET (GeV)",lowPhi + 2*ticksizex,etAxisMaximum,etAxisMaximum);

        // Draw a grid              
        int t = -1;
        for (double p = 0.; p <= 1.; p += 1. / 8.)
        {            
            // parallel to the right side       	           
            drawline(window, ag, 360*p+lowPhi, -5.0, 360*p, 5.0);
            if (++t % 2 == 0)
            {
                // a tick
                drawline(window,ag,360*p,5.0,360*p,5.0+ticksizey);
                String str = new Integer(((int) (360. * p))).toString();
                drawstring(window,ag,str,360*p-7.0,5.0+2.0*Math.sqrt(ticksizey));
            }
        }                      
        //Draw the title on phi axis
        drawstring(window,ag,AMath.PHI,0.6*360,5.0+3.0*Math.sqrt(ticksizey));

        for (double e = 0.; e <= 1.; e += 1. / 10.)
        {
            // parallel to the bottom
            drawline(window, ag, lowPhi*(1-e), 10.0*e-5.0, 360.0+lowPhi*(1-e), 10.0*e-5.0);
            // a tick
            drawline(window, ag, lowPhi*(1-e), 10.0*e-5.0, lowPhi*(1-e), 10.0*e-5.0+ticksizey);
            String str = new Integer((int)Math.round(e*10)-5).toString();
            if (AProjectionLegoPlot.reverse)
                str = new Integer((int)Math.round(-e*10)+5).toString();
            drawstring(window,ag,str,lowPhi*(1-e)-20, 10.0*e-5.0+3.0*Math.sqrt(ticksizey));
        }
        //Draw the title on eta axis
        drawstring(window,ag,AMath.ETA,0.5*lowPhi-30,+5.0*Math.sqrt(ticksizey));

        //reset font size
        ag.g.setFont(ag.g.getFont().deriveFont(originalFontSize));
    }

    public static void drawJet(double et, double eta, double phi, Color color, AWindow window, AGraphics ag, double radius)
    {    
        ag.setStroke(new BasicStroke(legoTowersWidth)); //Setting Lego Towers Width
        double x = (phi*360)/ AMath.TWO_PI;//x is phi converted from rad to deg
        int numPoints = 128;//points around circumference of circle
        double[][] hj = new double[2][numPoints];
        double[][] vj = new double[2][numPoints];
        int phiwrap = 0;
        double old_jetx = -1;
        int[] pj = new int[2];
        pj[0] = pj[1] = 0;
        for (int p = 0; p < numPoints; p++)
        {
            //add on fraction of diameter of circle in phi
            double jetx = x + radius*36 * Math.cos(AMath.TWO_PI * p / (numPoints - 1));
            if (jetx < 0)
                jetx += 360;
            if(jetx >= 360)
                jetx -= 360;
            if(old_jetx > 0) 
            {
                double jetx_diff = Math.abs(jetx - old_jetx);
                if(jetx_diff > 360 / 2)
                    phiwrap = (phiwrap + 1) % 2;
            }
            // remember the last jetx, to see if we've wrapped around next time
            old_jetx = jetx;
            double y = eta;
            if (AProjectionLegoPlot.reverse)
                y = -y;
            //add on fraction of diameter of circle in eta
            double jety = y + radius * Math.sin(AMath.TWO_PI * p / (numPoints - 1));
            jetx=AProjectionLegoPlot.adjustPhi(window,jetx,jety);//move phi value due to slant of eta axis
            Point2D.Double[] corners=window.getUserCorners();
            if(jetx < corners[0].x || jetx > corners[1].x || jety < corners[1].y || jety > corners[2].y)
                continue;//exits loop if not in limits of drawn plot
            hj[phiwrap][pj[phiwrap]] = jetx;
            vj[phiwrap][pj[phiwrap]] = jety;
            ++pj[phiwrap];
        }

        Point2D.Double p= new Point2D.Double(0,0);
        for(int i=0; i<pj[0]; i++)
        {
            p = window.calculateDisplay(hj[0][i],vj[0][i]);
            hj[0][i]=p.x;
            vj[0][i]=p.y;
        }
        p.x=0; p.y=0;
        for(int i=0; i<pj[1]; i++)
        {
            p = window.calculateDisplay(hj[1][i],vj[1][i]);
            hj[1][i]=p.x;
            vj[1][i]=p.y;
        }
        Stroke origStroke = ag.getGraphics2D().getStroke();        
        boolean drawFrames = AColorMap.drawFrames();
        //if drawing frames this part draws the frame otherwise draws the fill
        ag.setStroke(new BasicStroke(3));
        if(drawFrames)
            ag.setColor(AProjectionLegoPlot.defaultColorMap[AColorMap.BK]);
        else    
            ag.setColor(color);

        if (pj[0] > 2)
        {
            ag.drawPolygon(hj[0], vj[0], pj[0]);
        }
        if (pj[1] > 2)
        {
            ag.drawPolygon(hj[1], vj[1], pj[1]);
        }

        //this part will draw the fill color inside the frame if is not black otherwise already drawn 
        if(drawFrames && !color.equals(AProjectionLegoPlot.defaultColorMap[AColorMap.BK]))
        {
            ag.setStroke(new BasicStroke(1));
            ag.setColor(color);

            if (pj[0] > 2)
            {
                ag.drawPolygon(hj[0], vj[0], pj[0]);
            }
            if (pj[1] > 2)
            {
                ag.drawPolygon(hj[1], vj[1], pj[1]);
            }
        }

        //reset stroke
        ag.setStroke(origStroke);
    }

    public static void drawJetText(double et, double eta, double phi, Color color, AWindow window, AGraphics ag, double radius)
    {
        double x = (phi*360)/ AMath.TWO_PI;//x is phi converted from rad to deg
        int numPoints = 128;//points around circumference of circle
        double[][] hj = new double[2][numPoints];
        double[][] vj = new double[2][numPoints];
        int phiwrap = 0;
        double old_jetx = -1;
        int[] pj = new int[2];
        pj[0] = pj[1] = 0;
        for (int p = 0; p < numPoints; p++)
        {
            //add on fraction of diameter of circle in phi
            double jetx = x + radius*36 * Math.cos(AMath.TWO_PI * p / (numPoints - 1));
            if (jetx < 0)
                jetx += 360;
            if(jetx >= 360)
                jetx -= 360;
            if(old_jetx > 0) 
            {
                double jetx_diff = Math.abs(jetx - old_jetx);
                if(jetx_diff > 360 / 2)
                    phiwrap = (phiwrap + 1) % 2;
            }
            // remember the last jetx, to see if we've wrapped around next time
            old_jetx = jetx;
            double y = eta;
            if (AProjectionLegoPlot.reverse)
                y = -y;
            //add on fraction of diameter of circle in eta
            double jety = y + radius * Math.sin(AMath.TWO_PI * p / (numPoints - 1));
            jetx=AProjectionLegoPlot.adjustPhi(window,jetx,jety);//move phi value due to slant of eta axis
            Point2D.Double[] corners=window.getUserCorners();
            if(jetx < corners[0].x || jetx > corners[1].x || jety < corners[1].y || jety > corners[2].y)
                continue;//exits loop if not in limits of drawn plot
            hj[phiwrap][pj[phiwrap]] = jetx;
            vj[phiwrap][pj[phiwrap]] = jety;
            ++pj[phiwrap];
        }

        Point2D.Double p= new Point2D.Double(0,0);
        for(int i=0; i<pj[0]; i++)
        {
            p = window.calculateDisplay(hj[0][i],vj[0][i]);
            hj[0][i]=p.x;
            vj[0][i]=p.y;
        }

        //draw the jet et as text
        String s = Double.toString(et);
        int dot = s.indexOf('.');
        String ss = s.substring(0, dot+0);
        float originalFontSize= ag.g.getFont().getSize2D();
        float newFontSize= (float) 25.;
        ag.g.setFont(ag.g.getFont().deriveFont(newFontSize));
        ag.g.setColor(color);
        ag.drawString(" "+ss, hj[0][0], vj[0][0]);
        ag.g.setFont(ag.g.getFont().deriveFont(originalFontSize));
    }

    public static void drawMissEt(AWindow window, AGraphics ag, int phi, double pt, Color color, double maxEt)
    {
        //color
        ag.setColor(color);

        drawBox(window, ag, phi, 0, AProjectionLegoPlot.DRAW_MET, 0, pt, maxEt, 0.5);

        double phibinsize=360.0/AProjectionLegoPlot.nPhiCells;
        double[] h = new double[4];
        double[] v = new double[4];
        double etaChange = 0.5;
        for(double etaStart=-5.0; etaStart<5.0; etaStart+=2*etaChange)
        {
            v[0] = etaStart;
            h[0] = AProjectionLegoPlot.adjustPhi(window,phi*phibinsize,v[0]);
            v[1] = v[0]+etaChange;
            h[1] = AProjectionLegoPlot.adjustPhi(window,phi*phibinsize,v[1]);
            v[2] = v[1];
            h[2] = h[1] + phibinsize;
            v[3] = v[0];
            h[3] = h[0] + phibinsize;
            //convert to window coords
            Point2D.Double p= new Point2D.Double(0,0);
            for(int i=0; i<4; i++)
            {
                p = window.calculateDisplay(h[i],v[i]);
                h[i]=p.x;
                v[i]=p.y;
            }
            //draw
            ag.fillPolygon(h, v, 4);
        }
    }

    public static void drawBox(AWindow window, AGraphics ag, int phi, int eta, int colouring, double et1, double et2, double maxEt, double boxsize)
    {
        Color drawingColor;
        //if colouring < 0 ->AOD object, otherwise its cells
        switch(colouring){
        case AProjectionLegoPlot.DRAW_MET:
            drawingColor=AProjectionLegoPlot.defaultColorMap[parameterStore.get("ETMis", "Constant").getI()];
            break;
        case AProjectionLegoPlot.DRAW_MUON: 
            drawingColor=AProjectionLegoPlot.defaultColorMap[parameterStore.get("Muon", "Constant").getI()]; 
            break;
        case AProjectionLegoPlot.DRAW_ELECTRON: 
            drawingColor=AProjectionLegoPlot.defaultColorMap[parameterStore.get("Electron", "Constant").getI()]; 
            break;
        case AProjectionLegoPlot.DRAW_PHOTON: 
            drawingColor=AProjectionLegoPlot.defaultColorMap[parameterStore.get("Photon", "Constant").getI()]; 
            break;
        case AProjectionLegoPlot.DRAW_CLUSTER: 
            drawingColor=AProjectionLegoPlot.defaultColorMap[parameterStore.get("Cluster", "Constant").getI()]; 
            break;
        case AProjectionLegoPlot.DRAW_COMPOSITEPARTICLE: 
            drawingColor=AProjectionLegoPlot.defaultColorMap[parameterStore.get("CompositeParticle", "Constant").getI()]; 
            break;
        case AProjectionLegoPlot.DRAW_BJET: 
            drawingColor=AProjectionLegoPlot.defaultColorMap[parameterStore.get("BJet", "Constant").getI()];
            break;
        case AProjectionLegoPlot.DRAW_TAUJET: 
            drawingColor=AProjectionLegoPlot.defaultColorMap[parameterStore.get("TauJet", "Constant").getI()]; 
            break;
        case AProjectionLegoPlot.DRAW_JET: 
            drawingColor=AProjectionLegoPlot.defaultColorMap[parameterStore.get("Jet", "Constant").getI()]; 
            break;       
        default: drawingColor=AProjectionLegoPlot.caloColorMap[colouring];
        }

        int object = -colouring;//should be positive for "objects" and negative for calo cells

        //now check transparency option
        //result is transparent if colouring is < 0
        switch(parameterStore.get("LegoPlot", "TowerOutlines").getI()){
        //AOD: colouring is already correct
        case 0: break;
        //cells: reverse so now AOD solid
        case 1: colouring*=-1; break;
        //both
        case 2: colouring=-1; break;
        //neither
        case 3: colouring=1; break;
        }
        double phibinsize=360.0/AProjectionLegoPlot.nPhiCells;
        double etabinsize=10.0/AProjectionLegoPlot.nEtaCells;
        double x = phi*phibinsize;//x is real phi position
        double y = eta*etabinsize-5;//y is real eta position
        if (object>0) {phibinsize=360./64.; etabinsize=10./100.;}
        double x1 = AProjectionLegoPlot.adjustPhi(window,x, y);//x1 is real phi position
        double x2 = AProjectionLegoPlot.adjustPhi(window,x, y-etabinsize);//shifted x due to eta bin size
        double[][] h = new double[3][4];
        double[][] v = new double[3][4];
        //adjust the values of the energies
        et1=(1-AProjectionLegoPlot.getyz(window.getIndex()))*50*(et1/maxEt)*0.9;
        et2=et1+(1-AProjectionLegoPlot.getyz(window.getIndex()))*50*(et2/maxEt)*0.9;

        double phishift = boxsize * (phibinsize/2);     //to center towers with recpt eta-phi coordinates x1 and x2
        double etashift = boxsize * ((etabinsize)/2);

        //front
        h[0][0] = x1 - phishift;
        v[0][0] = y-et1 - etashift;   // (bottom front)
        h[0][1] = x1 - phishift;
        v[0][1] = y-et2 - etashift;
        h[0][2] = x1 + phishift;
        v[0][2] = y-et2 - etashift;
        h[0][3] = x1 + phishift;
        v[0][3] = y-et1 - etashift;
        //top
        h[1][0] = x2 - phishift;
        v[1][0] = y-et2-  etashift-etashift;    //SHIFT to make top squared
        h[1][1] = x1 - phishift;
        v[1][1] = y-et2 - etashift;
        h[1][2] = x1 + phishift;
        v[1][2] = y-et2 - etashift;
        h[1][3] = x2 + phishift;
        v[1][3] = y-et2-  etashift-etashift;    //SHIFT to make top squared
        //side
        h[2][0] = x2 - (phishift);            
        v[2][0] = y-(et2)- etashift -etashift; // top back SHIFT to make top squared
        h[2][1] = x1 - phishift;
        v[2][1] = y-et2 - etashift; // top front
        h[2][2] = x1 - phishift;
        v[2][2] = y-(et1) - etashift; // bottom front
        h[2][3] = x2 - (phishift);          
        v[2][3] = y-et1 -(etashift);  // bottom back


        //convert to window coords
        Point2D.Double p= new Point2D.Double(0,0);
        for(int i=0; i<4; i++)
        {
            p = window.calculateDisplay(h[0][i],v[0][i]);
            h[0][i]=p.x;
            v[0][i]=p.y;
            p.x=0; p.y=0;
            p = window.calculateDisplay(h[1][i],v[1][i]);
            h[1][i]=p.x;
            v[1][i]=p.y;
            p.x=0; p.y=0;
            p = window.calculateDisplay(h[2][i],v[2][i]);
            h[2][i]=p.x;
            v[2][i]=p.y;
        }
        
        //draw sides
        ag.setColor(drawingColor);
		if (colouring>=0) 
			ag.fillPolygon(h[2], v[2], 4);       	
        else if(colouring == AProjectionLegoPlot.DRAW_JET){
        	ag.fillPolygon(h[2], v[2], 4, (float)APar.instance().get("Jet","AlphaJets").getD());    //f specifies transparency fraction
    	}
    	else
    		ag.fillPolygon(h[2], v[2], 4, 0.8f);    //f specifies transparency fraction	
            	    	  	  	  	     				
		//colour front
        ag.setColor(drawingColor);
        //draw front        
        if (colouring>=0) 
        	ag.fillPolygon(h[0], v[0], 4);               	        	
    	else if(colouring == AProjectionLegoPlot.DRAW_JET){
        	ag.fillPolygon(h[0], v[0], 4, (float)APar.instance().get("Jet","AlphaJets").getD());    //f specifies transparency fraction
        }
        else
        	ag.fillPolygon(h[0], v[0], 4, 0.8f);    //f specifies transparency fraction	
            	                      
        //colour top
        Color ddc = drawingColor.darker().darker();
        ag.setColor(new Color(ddc.getRed(),ddc.getGreen(),ddc.getBlue()));
        //draw top
        if (colouring>=0) 
            ag.fillPolygon(h[1], v[1], 4);
        else if(colouring == AProjectionLegoPlot.DRAW_JET){
        	ag.fillPolygon(h[1], v[1], 4, (float)APar.instance().get("Jet","AlphaJets").getD());    //f specifies transparency fraction
        }
        else
        	ag.fillPolygon(h[1], v[1], 4, 0.8f);    //f specifies transparency fraction	       	        

    }    

    //draw box method that can be used when not all the towers of certain object have assigned the same colour - the parameter color specifies the i color component
    public static void drawBox(AWindow window, AGraphics ag, int phi, int eta, int colouring, Color color, double et1, double et2, double maxEt, double boxsize)
    { 
        Color drawingColor;
        switch(colouring){
        case AProjectionLegoPlot.DRAW_MET:        
            drawingColor= color;
            break;
        case AProjectionLegoPlot.DRAW_MUON:         
            drawingColor= color;
            break;
        case AProjectionLegoPlot.DRAW_ELECTRON:         
            drawingColor= color;
            break;
        case AProjectionLegoPlot.DRAW_PHOTON:         
            drawingColor= color;
            break;
        case AProjectionLegoPlot.DRAW_CLUSTER: 
            drawingColor= color;
            break;
        case AProjectionLegoPlot.DRAW_COMPOSITEPARTICLE: 
            drawingColor= color;
            break;
        case AProjectionLegoPlot.DRAW_BJET:         
            drawingColor= color;
            break;
        case AProjectionLegoPlot.DRAW_TAUJET:         
            drawingColor= color;
            break;
        case AProjectionLegoPlot.DRAW_JET:         
            drawingColor= color;
            break;
        default: drawingColor=AProjectionLegoPlot.caloColorMap[colouring];
        }

        int object = -colouring;//should be positive for "objects" and negative for calo cells

        //now check transparency option
        //result is transparent if colouring is < 0
        switch(parameterStore.get("LegoPlot", "TowerOutlines").getI()){
        //AOD: colouring is already correct
        case 0: break;
        //cells: reverse so now AOD solid
        case 1: colouring*=-1; break;
        //both
        case 2: colouring=-1; break;
        //neither
        case 3: colouring=1; break;
        }
        double phibinsize=360.0/AProjectionLegoPlot.nPhiCells;
        double etabinsize=10.0/AProjectionLegoPlot.nEtaCells;
        double x = phi*phibinsize;//x is real phi position
        double y = eta*etabinsize-5;//y is real eta position
        if (object>0) {phibinsize=360./64.; etabinsize=10./100.;}
        double x1 = AProjectionLegoPlot.adjustPhi(window,x, y);//x1 is real phi position
        double x2 = AProjectionLegoPlot.adjustPhi(window,x, y-etabinsize);//shifted x due to eta bin size
        double[][] h = new double[3][4];
        double[][] v = new double[3][4];
        //adjust the values of the energies
        et1=(1-AProjectionLegoPlot.getyz(window.getIndex()))*50*(et1/maxEt)*0.9;
        et2=et1+(1-AProjectionLegoPlot.getyz(window.getIndex()))*50*(et2/maxEt)*0.9;

        double phishift = boxsize * (phibinsize/2);     //to center towers with recpt eta-phi coordinates x1 and x2
        double etashift = boxsize * ((etabinsize)/2);

        //front
        h[0][0] = x1 - phishift;
        v[0][0] = y-et1 - etashift;   // (bottom front)
        h[0][1] = x1 - phishift;
        v[0][1] = y-et2 - etashift;
        h[0][2] = x1 + phishift;
        v[0][2] = y-et2 - etashift;
        h[0][3] = x1 + phishift;
        v[0][3] = y-et1 - etashift;
        //top
        h[1][0] = x2 - phishift;
        v[1][0] = y-et2-  etashift-etashift;    //SHIFT to make top squared
        h[1][1] = x1 - phishift;
        v[1][1] = y-et2 - etashift;
        h[1][2] = x1 + phishift;
        v[1][2] = y-et2 - etashift;
        h[1][3] = x2 + phishift;
        v[1][3] = y-et2-  etashift-etashift;    //SHIFT to make top squared
        //side
        h[2][0] = x2 - (phishift);            
        v[2][0] = y-(et2)- etashift -etashift; // top back SHIFT to make top squared
        h[2][1] = x1 - phishift;
        v[2][1] = y-et2 - etashift; // top front
        h[2][2] = x1 - phishift;
        v[2][2] = y-(et1) - etashift; // bottom front
        h[2][3] = x2 - (phishift);          
        v[2][3] = y-et1 -(etashift);  // bottom back

        //convert to window coords
        Point2D.Double p= new Point2D.Double(0,0);
        for(int i=0; i<4; i++)
        {
            p = window.calculateDisplay(h[0][i],v[0][i]);
            h[0][i]=p.x;
            v[0][i]=p.y;
            p.x=0; p.y=0;
            p = window.calculateDisplay(h[1][i],v[1][i]);
            h[1][i]=p.x;
            v[1][i]=p.y;
            p.x=0; p.y=0;
            p = window.calculateDisplay(h[2][i],v[2][i]);
            h[2][i]=p.x;
            v[2][i]=p.y;
        }

		//draw sides
		ag.setColor(drawingColor);
		if (colouring>=0) 
			ag.fillPolygon(h[2], v[2], 4);       	
		else if(colouring == AProjectionLegoPlot.DRAW_JET){       	
	    	ag.fillPolygon(h[2], v[2], 4, (float)APar.instance().get("Jet","AlphaJets").getD());    
	    }else if (colouring == AProjectionLegoPlot.DRAW_BJET) {
	       	ag.fillPolygon(h[2], v[2], 4, 0.4f);    
	    }else
	    	ag.fillPolygon(h[2], v[2], 4, 0.8f);    //f specifies transparency fraction	
		    	    	  	  	  	     				
		//colour front
		ag.setColor(drawingColor);
		//draw front        
		if (colouring>=0) 
			ag.fillPolygon(h[0], v[0], 4);
		else if(colouring == AProjectionLegoPlot.DRAW_JET){      	
		    ag.fillPolygon(h[0], v[0], 4, (float)APar.instance().get("Jet","AlphaJets").getD());
		}else if(colouring == AProjectionLegoPlot.DRAW_BJET) {
		    ag.fillPolygon(h[0], v[0], 4, 0.4f);    
		}else
		    ag.fillPolygon(h[0], v[0], 4, 0.8f);    //f specifies transparency fraction	
		    	                      
		//colour top
		Color ddc = drawingColor.darker().darker();
		ag.setColor(new Color(ddc.getRed(),ddc.getGreen(),ddc.getBlue()));
		//draw top
		if (colouring>=0) 
			ag.fillPolygon(h[1], v[1], 4);
		else if(colouring == AProjectionLegoPlot.DRAW_JET){   		
			ag.fillPolygon(h[1], v[1], 4, (float)APar.instance().get("Jet","AlphaJets").getD());    //f specifies transparency fraction
		}else  if(colouring == AProjectionLegoPlot.DRAW_BJET) {
			ag.fillPolygon(h[1], v[1], 4, 0.4f);    //f specifies transparency fraction
		}
		else
			ag.fillPolygon(h[1], v[1], 4, 0.8f);    //f specifies transparency fraction	       	        
	}    
}
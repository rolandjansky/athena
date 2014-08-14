package atlantis.interactions;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Iterator;

import java.awt.GridLayout;
import java.awt.Point;
import java.awt.Graphics2D;
import java.awt.Rectangle;
import java.awt.Dimension;
import java.awt.Polygon;
import java.awt.event.ActionListener;
import java.awt.event.ActionEvent;
import java.awt.geom.Point2D;
import java.awt.geom.Ellipse2D;

import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JMenuItem;
import javax.swing.JButton;

import atlantis.canvas.ACanvas;
import atlantis.canvas.AWindow;
import atlantis.graphics.ADrawnGraphics2D;
import atlantis.graphics.AGraphics;
import atlantis.graphics.ACoord;
import atlantis.graphics.dnd.ADnDButton;
import atlantis.graphics.dnd.ADnDLabel;
import atlantis.graphics.dnd.ADragListener;
import atlantis.gui.AGUI;
import atlantis.gui.ALogPane;
import atlantis.list.AListManager;
import atlantis.list.AList;
import atlantis.output.AExceptionHandler;
import atlantis.output.AOutput;
import atlantis.utils.ALogger;
import atlantis.utils.AMath;



/**
 * The superclass of all the selection (rubberband) interactions.
 */
public abstract class ASelection extends AInteraction
                                 implements ASleepMouseDragListener, ActionListener
{

  private static ALogger logger = ALogger.getLogger(ASelection.class);
    
  private JPanel actionsPanel;
  private GridLayout actionsPanelLayout;
  private int d = 10;
  private boolean[][] map;
  protected final static int radius = 10;
  protected int region;
  protected boolean isValid = false;


  public final static String ZOOM = "Zoom";
  public final static String NEW_LIST = "New List";
  public final static String SUMMARIZE = "Summarize";



  public ASelection(int hotRegionsCount)
  {
      super(hotRegionsCount, AUTOMATIC_REPAINT, SCREEN_COORDINATES, true);

      for(int i = 0; i < hr.length; i++)
      {
          hr[i] = new Ellipse2D.Double(0, 0, radius, radius);
      }

      actionsPanelLayout = new GridLayout(0, 1);
      actionsPanel = new JPanel(actionsPanelLayout);


      addActionButton(ZOOM, new ADragListener()
      {
          public void dragPerformed(Object from, Object to, int action)
          {
              if(to instanceof ADnDLabel)
              {
                  zoomIn(((ADnDLabel) to).getText());
              }
              else if(to instanceof AWindow)
              {
                  zoomIn(((AWindow) to).getName());
              }
              invalidate();
          }
      },
          new ActionListener()
          {
              public void actionPerformed(ActionEvent e)
              {
                  window.saveCorners();
                  window.setUserCorners(getCorners());
                  invalidate();
              }
      });


      addActionButton(NEW_LIST, new ActionListener()
      {
          public void actionPerformed(ActionEvent e)
          {
              if(e.getActionCommand().equals(NEW_LIST))
              {
                  Point2D.Double[] zoomedCorners = getCorners();
                  Point2D.Double[] userCorners = window.getUserCorners();
                  window.setUserCornersNoRepaint(zoomedCorners);
                  Rectangle bounds = window.getBounds();
                  bounds.setLocation(0, 0);
                  window.paintWindowFromScratch(new ADrawnGraphics2D(bounds));
                  ADrawnGraphics2D.updateLastDrawn();
                  AListManager.getInstance().copyLastDrawn();
                  AListManager.getInstance().showLists();
                  window.setUserCornersNoRepaint(userCorners);
              }
          }
      });


      addActionButton(SUMMARIZE, new ActionListener()
      {
          public void actionPerformed(ActionEvent e)
          {
              if(e.getActionCommand().equals(SUMMARIZE))
              {
                  Point2D.Double[] zoomedCorners = getCorners();
                  Point2D.Double[] userCorners = window.getUserCorners();
                  window.setUserCornersNoRepaint(zoomedCorners);
                  Rectangle bounds = window.getBounds();
                  bounds.setLocation(0, 0);
                  window.paintWindowFromScratch(new ADrawnGraphics2D(bounds));
                  ADrawnGraphics2D.updateLastDrawn();
                  AListManager.getInstance().summarize();
                  window.setUserCornersNoRepaint(userCorners);
              }
          }
      });

  } // ASelection() ---------------------------------------------------------




  public void invalidate()
  {
      isValid = false;
      actionsPanel.setVisible(false);
  }


  public void addActionButton(String buttonName, ADragListener dragList,
                              ActionListener clickList)
  {
      ADnDButton b = new ADnDButton(buttonName);
      b.setFocusPainted(false);
      b.addActionListener(clickList);
      b.addDragListener(dragList);
      actionsPanelLayout.setRows(actionsPanelLayout.getRows() + 1);
      actionsPanel.add(b);
      actionsPanel.setSize(actionsPanel.getPreferredSize());
  }


  public void addActionButton(String buttonName, ActionListener clickListener)
  {
      JButton b = new JButton(buttonName);
      b.setFocusPainted(false);
      b.addActionListener(clickListener);
      actionsPanelLayout.setRows(actionsPanelLayout.getRows() + 1);
      actionsPanel.add(b);
      actionsPanel.setSize(actionsPanel.getPreferredSize());
  }


  public JMenuItem[] getPopupItems()
  {
      ArrayList<JMenuItem> al = new ArrayList<JMenuItem>();
      if(window.getUnzoomPossible())
      {
          JMenuItem it = new JMenuItem("Unzoom");
          it.addActionListener(new ActionListener()
          {
              public void actionPerformed(ActionEvent e)
              {
                  window.unzoom();
              }
          });
          al.add(it);
      }
      if(window.getUnzoomFullPossible())
      {
          JMenuItem it = new JMenuItem("Unzoom Full");
          it.addActionListener(new ActionListener()
          {
              public void actionPerformed(ActionEvent e)
              {
                  window.unzoomFull();
              }
          });
          al.add(it);
      }
      // Can't cast arrays very easily...
      JMenuItem[] ret = new JMenuItem[al.size()];
      for(int i = 0; i < al.size(); i++)
      {
          ret[i] = al.get(i);
      }
      return ret;

  }


  public void actionPerformed(ActionEvent e)
  {
      String action = e.getActionCommand();
      if(action.equals("Unzoom"))
      {
          window.unzoom();
      }
  }


  public int getPopupType()
  {
      return APopupListener.WINDOW_POPUP;
  }


  public void connect(AInteractionsManager manager)
  {
      super.connect(manager);
      window.getView().add(actionsPanel);
      if(isValid)
      {
          actionsPanel.setVisible(true);
      }
      else
      {
          actionsPanel.setVisible(false);
      }
      window.validate();
  }


  public void disconnect()
  {
      window.getView().remove(actionsPanel);
  }


  public int getButton()
  {
      return AInteractionsManager.LEFT;
  }


  public void start(Point2D.Double p, int region, int key)
  {
      this.region = region;
      actionsPanel.setVisible(false);
  }


  public void cancel() {}


  private boolean canLayout(int i0, int j0)
  {
      Dimension panelSize = actionsPanel.getSize();
      panelSize.width = (int) Math.ceil((panelSize.width + d) / (double) d);
      panelSize.height = (int) Math.ceil((panelSize.height + d) / (double) d);
      for(int j = 0; j < panelSize.height; j++)
      {
          for(int i = 0; i < panelSize.width; i++)
          {
              if(i0 + i > map[0].length - 1)return false;
              if(j0 + j > map.length - 1)return false;
              if(map[j0 + j][i0 + i] == false)return false;
          }
      }
      return true;

  }


  private Point calculateBestPosition()
  {
      AWindow w = window;
      Dimension size = w.getView().getSize();
      int ny = (int) Math.ceil(size.height / (double) d);
      int nx = (int) Math.ceil(size.width / (double) d);
      map = new boolean[ny][nx];
      Polygon p = new Polygon();
      for(int i = 0; i < 4; i++)
      {
          p.addPoint((int) hr[i].getCenterX(), (int) hr[i].getCenterY());
      }

      for(int j = 0; j < map.length; j++)
      {
          for(int i = 0; i < map[0].length; i++)
          {
              if(p.intersects(i * d, j * d, d, d) || p.contains(i * d, j * d, d, d))
              {
                  map[j][i] = false;
              }
              else
              {
                  map[j][i] = true;
              }
          }
      }

      for(int j = 0; j < map.length; j++)
      {
          for(int i = 0; i < map[0].length; i++)
          {
              if(canLayout(i, j))
              {
                  map[j][i] = true;
              }
              else
              {
                  map[j][i] = false;
              }
          }
      }

      int x0 = (int) hr[region].getMaxX();
      int y0 = (int) hr[region].getMaxY();
      int i_min = 0, j_min = 0;
      double dist, dist_min = 1e10;
      for(int j = 0; j < map.length; j++)
      {
          for(int i = 0; i < map[0].length; i++)
          {
              if(map[j][i] == true)
              {
                  dist = Math.hypot(x0 - i*d, y0 - j*d);
                  if(dist < dist_min)
                  {
                      i_min = i;
                      j_min = j;
                      dist_min = dist;
                  }
              }
          }
      }

      return new Point(i_min * d + d / 2, j_min * d + d / 2);
  }


  public void stop()
  {
      if(isValid)
      {
          Point p = calculateBestPosition();
          actionsPanel.setLocation(p.x, p.y);
          actionsPanel.setVisible(true);
          window.revalidate();
      }
  }


  protected int getUpperLeftRegion()
  {
      int point = 0;
      double savedValue = getRadius(hr[0]);
      for(int i = 1; i < 4; i++)
      {
          double value = getRadius(hr[i]);
          if(value < savedValue)
          {
              savedValue = value;
              point = i;
          }
      }
      return point;
  }


  protected Point2D.Double[] convert(
      double x0, double y0, double x1, double y1, double x2, double y2)
  {
      if(!isValid)
      {
          return window.getUserCorners();
      }
      if(Math.abs(getAPolygon().getArea()) == 0)
      {
          return window.getUserCorners();
      }

      double[][][] hv = new double[2][1][3];
      hv[0][0][0] = x0;
      hv[1][0][0] = y0;
      hv[0][0][1] = x1;
      hv[1][0][1] = y1;
      hv[0][0][2] = x2;
      hv[1][0][2] = y2;
      ACoord d = window.calculateUser(new ACoord(hv));
      Point2D.Double[] corners = new Point2D.Double[3];
      for(int i = 0; i < 3; i++)
      {
          corners[i] = new Point2D.Double(d.hv[0][0][i], d.hv[1][0][i]);
      }

      return corners;
  }


  protected Point2D.Double[] convert(int region1, int region2, int region3)
  {
      return convert(hr[region1].getCenterX(), hr[region1].getCenterY(),
                     hr[region2].getCenterX(),
                     hr[region2].getCenterY(), hr[region3].getCenterX(),
                     hr[region3].getCenterY());
  }


  public abstract Point2D.Double[] getCorners();


  protected void paintStandard(Graphics2D g)
  {
      if(isValid)
      {
          AGraphics ag = AGraphics.makeAGraphics(g);
          ag.updateDrawParameters(frameDrawParameters);
          ag.drawPolygon(getAPolygon());
          drawLine(4, 5, ag);
          ag.updateDrawParameters(drawParameters);
          ag.drawPolygon(getAPolygon());
          drawLine(4, 5, ag);
          drawActivePoint(region, ag);
      }
  }


  public void zoomIn(String zoomWindow)
  {
      String currentWindow = window.getName();
      ACanvas.getCanvas().copyWindowSettings(currentWindow, zoomWindow);
      ACanvas.getCanvas().getWindow(zoomWindow).setUserCorners(getCorners());
  }

} // class ASelection =======================================================

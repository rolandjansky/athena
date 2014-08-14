package atlantis.graphics;

import java.awt.geom.Point2D;
import java.util.ArrayList;
import java.util.List;

import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;

import atlantis.parameters.APar;
import static org.junit.Assert.*;

public class ACoordTest {
	private static final double TOLERANCE = 0.001;

	@BeforeClass
	public static void setUpOnce() throws Exception {
		APar.constructDummyForTesting();
	}

	@Test
	public void constructACoordWithOnePoint() {
		ACoord coord = new ACoord(0.0, 0.0, 0);
		Point2D.Double point = new Point2D.Double(0.0,0.0);
		assertContainsPoints(coord,point);
	}
	
	@Test
	public void constructACoordWithOneArrayOfPoints() {
		double[] h = {0.0, 1.0, -1.0, 2.5};
		double[] v = {0.0, 0.0, 1.0, -2.5};
		ACoord coord = new ACoord(h,v);
		Point2D.Double[] expectedPoints = createPointsArray(h,v);
		assertContainsPoints(coord,expectedPoints);
	}

	/**
	 * Currently just checks first set of points, i.e. x = hv[0][0][i], y = [1][0][i]
	 * @param coord
	 * @param points
	 */
	protected static void assertContainsPoints(ACoord coord, Point2D.Double... points) {
		double[] x = coord.hv[0][0];
		double[] y = coord.hv[1][0];
		int xLen = x.length;
		int yLen = y.length;
		assertTrue(String.format(
				"Numbers of x and y coordinates are not equal: %d, %d",xLen,yLen),
				xLen==yLen);
		assertTrue(String.format(
				"Wrong number of points: expected %d, found %d",points.length,xLen),
				points.length==xLen);
		int i=0;
		for (Point2D.Double point : points) {
			Point2D.Double hvPoint = new Point2D.Double(x[i],y[i]);
			double distance = hvPoint.distance(point);
			if (distance>TOLERANCE) {
				fail(String.format("Didn't find expected points. First difference is point %d: expected (%f,%f), found (%f,%f)",
						i,point.x,point.y,hvPoint.x,hvPoint.y));
			}
			i++;
		}
	}
	
	private Point2D.Double[] createPointsArray(double[] x, double[] y) {
		Point2D.Double[] points = new Point2D.Double[x.length];
		for (int i=0; i<x.length; i++) {
			points[i] = new Point2D.Double(x[i], y[i]);
		}
		return points;
	}
}

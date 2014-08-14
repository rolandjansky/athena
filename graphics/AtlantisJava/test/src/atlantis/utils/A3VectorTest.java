package atlantis.utils;

import static org.junit.Assert.*;

import org.junit.Test;

/**
 * Tests for A3Vector class. Initial rather clumsy implementation just to get started.
 * @author waugh
 *
 */
public class A3VectorTest {
	/** Arbitrary tolerance for floating-point comparisons */
	private static final double TOLERANCE = 0.00001;

	@Test
	public void testA3Vector() {
		A3Vector v = new A3Vector();
		assertA3VectorEquals("three-vector",0.0,0.0,0.0,v,TOLERANCE);
	}

	@Test
	public void testA3VectorDoubleDoubleDouble() {
		A3Vector v = new A3Vector(1.0,-2.5,3.3);
		assertA3VectorEquals("three-vector",1.0,-2.5,3.3,v,TOLERANCE);
	}

	@Test
	public void testFromEtaPhiR() {
		A3Vector v = A3Vector.fromEtaPhiR(0.0, Math.PI, 2.0);
		assertA3VectorEquals("three-vector",-2.0,0.0,0.0,v,TOLERANCE);
	}

	@Test
	public void testFromRhoPhiZ() {
		A3Vector v = A3Vector.fromRhoPhiZ(1.0, Math.PI, 1.0);
		assertA3VectorEquals("three-vector",-1.0,0.0,1.0,v,TOLERANCE);
	}

	@Test
	public void testGetNormalized() {
		A3Vector v0 = new A3Vector(1.0,1.0,1.0);
		double a = 1 / Math.sqrt(3);
		A3Vector v = v0.getNormalized();
		assertA3VectorEquals("three-vector",a,a,a,v,TOLERANCE);
	}

	@Test
	public void testNormalize() {
		A3Vector v = new A3Vector(1.0,1.0,1.0);
		v.normalize();
		double a = 1 / Math.sqrt(3);
		assertA3VectorEquals("three-vector",a,a,a,v,TOLERANCE);
	}

	@Test
	public void testMagnitude() {
		A3Vector v = new A3Vector(1.0,1.0,1.0);
		double r = v.magnitude();
		double rExpected = Math.sqrt(3);
		assertEquals("magnitude calculation",rExpected,r,TOLERANCE);
	}

	@Test
	public void testScale() {
		A3Vector v = new A3Vector(1.0,1.0,1.0);
		v.scale(0.5);
		assertA3VectorEquals("three-vector",0.5,0.5,0.5,v,TOLERANCE);
	}

	@Test
	public void testAdd() {
		A3Vector v = new A3Vector(1.0,2.0,3.0);
		A3Vector v0 = new A3Vector(0.0,1.0,-0.5);
		v.add(v0);
		assertA3VectorEquals("three-vector",1.0,3.0,2.5,v,TOLERANCE);
	}

	@Test
	public void testSubtract() {
		A3Vector v = new A3Vector(1.0,2.0,3.0);
		A3Vector v0 = new A3Vector(0.0,1.0,-0.5);
		v.subtract(v0);
		assertA3VectorEquals("three-vector",1.0,1.0,3.5,v,TOLERANCE);
	}

	/**
	 * Check given A3Vector has expected components
	 */
	private static void assertA3VectorEquals(String message, double x, double y, double z,
			A3Vector v, double delta) {
		if (Math.abs(v.x-x)>delta || Math.abs(v.y-y)>delta || 
				Math.abs(v.z-z)>delta) {
			String vExpected = String.format("<%f,%f,%f>", x,y,z);
			String vActual = String.format("<%f,%f,%f>", v.x,v.y,v.z);
			String summary = message+" "+
				"expected:"+vExpected+" but was:"+vActual;
			fail(summary);
		}
	}
}

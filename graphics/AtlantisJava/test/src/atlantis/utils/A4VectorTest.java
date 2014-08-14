package atlantis.utils;


import static org.junit.Assert.*;

import org.junit.Test;

/**
 * Tests for A4Vector class.
 */
public class A4VectorTest {
	/** Arbitrary tolerance for floating-point comparisons */
	private static final double TOLERANCE = 0.00001;

	@Test
	public void testA4Vector() {
		A4Vector v = new A4Vector();
		assertA4VectorEquals("construct zero 4-vector",0.0,0.0,0.0,0.0,v,TOLERANCE);
	}

	@Test
	public void testA4VectorDoubleDoubleDoubleDouble() {
		// fourth argument is mass, not energy
		// use 3-4-5 numbers for simple arithmetic
		A4Vector v = new A4Vector(2.0,2.0,-1.0,4.0);
		assertA4VectorEquals("construct 4-vector",2.0,2.0,-1.0,5.0,v,TOLERANCE);
	}

	@Test
	public void testA4VectorA3VectorDouble() {
		A3Vector v3 = new A3Vector(2.0,2.0,-1.0);
		A4Vector v = new A4Vector(v3,4.0);
		assertA4VectorEquals("construct 4-vector",2.0,2.0,-1.0,5.0,v,TOLERANCE);
	}

	@Test
	public void testSet() {
		A4Vector v = new A4Vector();
		v.set(2.0,2.0,-1.0,4.0);
		assertA4VectorEquals("set 4-vector",2.0,2.0,-1.0,5.0,v,TOLERANCE);
	}
	
	@Test
	public void testSetPtEtaPhiM() {
	       A4Vector v1 = new A4Vector();
	       v1.setPtEtaPhiM(1.0, 0.0, 0.0, 0.0);
	       assertA4VectorEquals("set 4-vector",1.0,0.0,0.0,1.0,v1,TOLERANCE);
	       // test with negative pT
	       A4Vector v2 = new A4Vector();
           v2.setPtEtaPhiM(-1.0, 0.0, 0.0, 0.0);
           assertA4VectorEquals("set 4-vector",1.0,0.0,0.0,1.0,v2,TOLERANCE);
	}

	@Test
	public void testGetP() {
		A4Vector v = new A4Vector(2.0,2.0,-1.0,4.0);
		double p = v.getP();
		assertEquals(3.0,p,TOLERANCE);
	}

	@Test
	public void testGetPt() {
		A4Vector v = new A4Vector(0.3,0.4,-1.0,4.0);
		double pt = v.getPt();
		assertEquals(0.5,pt,TOLERANCE);
	}

	@Test
	public void testGetE() {
		A4Vector v = new A4Vector(2.0,2.0,-1.0,4.0);
		double e = v.getE();
		assertEquals(5.0,e,TOLERANCE);
	}
	
	@Test
	public void testgetEt() {
		A4Vector v = new A4Vector(1.0,1.0,2.0,3.0);
		double et = v.getEt();
		double x = Math.sqrt(5.0);
		assertEquals(x,et,TOLERANCE);
	}

	@Test
	public void testGetMass() {
		A4Vector v = new A4Vector(2.0,2.0,-1.0,4.0);
		double m = v.getMass();
		assertEquals(4.0,m,TOLERANCE);
	}
	
	@Test
	public void testGetMt() {
		A4Vector v = new A4Vector(1.0,1.0,2.0,3.0);
		double mt = v.getMt();
		double x = Math.sqrt(3.0);
		assertEquals(x,mt,TOLERANCE);
	}

	@Test
	public void testAddDoubleDoubleDoubleDouble() {
		A4Vector v = new A4Vector(2.0,2.0,-1.0,4.0);
		v.add(2.2,2.2,-1.1,4.4);
		assertA4VectorEquals("add 4-vectors",4.2,4.2,-2.1,10.5,v,TOLERANCE);
	}

	@Test
	public void testAddA4Vector() {
		A4Vector v = new A4Vector(2.0,2.0,-1.0,4.0);
		A4Vector v2 = new A4Vector(2.2,2.2,-1.1,4.4);
		v.add(v2);
		assertA4VectorEquals("add 4-vectors",4.2,4.2,-2.1,10.5,v,TOLERANCE);
	}
	
	/**
	 * Check given A4Vector has expected components
	 */
	private static void assertA4VectorEquals(String message, double px, double py, double pz, double e,
			A4Vector v, double delta) {
		if (Math.abs(v.px-px)>delta || Math.abs(v.py-py)>delta || 
				Math.abs(v.pz-pz)>delta || Math.abs(v.e-e)>delta) {
			String vExpected = String.format("<%f,%f,%f,%f>", px,py,pz,e);
			String vActual = String.format("<%f,%f,%f,%f>", v.px,v.py,v.pz,v.e);
			String summary = message+" "+
				"expected:"+vExpected+" but was:"+vActual;
			fail(summary);
		}
	}

}

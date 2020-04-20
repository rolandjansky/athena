# The Pufit Technique

## Introduction

The pufit algorithm (`pufit` during 2015-18 running) proved an important tool in
reducing the sensitivity of the MET trigger to pile-up. The original trigger
algorithm has been documented in the past, however the underlying technique can
be used more widely than just in this algorithm.

This underlying technique is the focus of this document. This is also designed
to be closer to the code than the existing documentation, so it should be
possible to understand how the code as written relates to the algorithm as
documented.


## The pufit technique

The pufit algorithm takes certain parts of the event deemed to be coming from
the hard-scatter vertex and derives corrections to them that are designed to
account for underlying contributions from pile-up or other sources of imbalance
such as the imbalanced caused by removing only part of a pile-up event.

The two constraints are

1. that the vector sum over 'pile-up' energy contributions and the derived
   corrections should be 0 (pile-up events do not carry real MET).
2. that the magnitudes of the corrections should be similar to the average
   pileup transverse energy density across the detector (pile-up is assumed to
   be approximately linear).

This can be expressed mathematically as

```math
  \chi^2 = s{\mathbf{\Delta}_\mu}^\mathrm{T}{V_\mu}^{-1}{\mathbf{\Delta}_\mu} +
  \Sigma_j\frac{{\Delta_j}^2}{V_\mathrm{tower}}
```
where

```math
  \mathbf{\Delta}_\mu = \mathbf{E_T}_\mu + \Sigma_i \mathcal{E}_i \mathbf{\Phi}_i
```
implements the first constraint and

```math
  \Delta_j = \mathcal{E}_j - {\langle E_T \rangle}_\mathrm{tower}
```
implements the second constraint.

$`\mathcal{E}_i`$ is the correction to the $`i\mathrm{th}`$ hard-scatter
object and

```math
  \mathbf{\Phi}_i = \left(\begin{matrix}\mathrm{cos}\phi_i \\
  \mathrm{sin}\phi_i\end{matrix}\right)
```
where $`\phi_i`$ is the azimuthal angle of the $`i\mathrm{th}`$
hard-scatter object.

The weights of the constraints are given by the corresponding variances. For the
first this is given by the covariance matrix of the sum over pileup objects,
that is

```math
  \mathbf{E_T}_\mu = \Sigma_{i \in \mu}\, \mathbf{p_T}_i\\
  V_\mu = \Sigma_{i \in \mu}\, \sigma_i \mathbf{\Phi}_i{\mathbf{\Phi}^\mathrm{T}}_i
```

where $`\sigma_i`$ is an estimate of the squared resolution of the
$`i\mathrm{th}`$ pileup object.

For the second, the mean and variance of the pileup tower distribution
($`{\langle E_T \rangle}_\mathrm{tower}`$ and $`V_\mathrm{tower}`$) must
be calculated separately.

$`s`$ is a parameter of the algorithm that describes the relative importance
of the two constraints.

The derived corrections are those that solve the equation

```math
  \frac{\partial}{\partial \mathcal{E}_k} \chi^2 = 0 \\
    = 2s\frac{\partial {\mathbf{\Delta}_\mu}^\mathrm{T}}{\partial \mathcal{E}_k}
  {V_\mu}^{-1}\mathbf{\Delta}_\mu + 2\Sigma_j\frac{\partial \Delta_j}{\partial
  \mathcal{E}_k}\frac{\Delta_j}{V_\mathrm{tower}}.
```
Using the expressions for $`\mathbf{\Delta}_\mu`$ and $`\Delta_i`$ this
yields

```math
  \frac{\partial {\mathbf{\Delta}_\mu}^\mathrm{T}}{\partial \mathcal{E}_k} = \mathbf{\Phi}_k \\
  \frac{\partial \Delta_j}{\partial \mathcal{E}_k} = \delta_{jk} \\
  \frac{\partial}{\partial \mathcal{E}_k} \chi^2 =
  2(s{\mathbf{\Phi}_k}^\mathrm{T}{V_\mu}^{-1}\mathbf{\Delta}_\mu + \delta_{kj}\frac{\Delta_j}{V_\mathrm{tower}})
```
and therefore

```math
  0 = s{\mathbf{\Phi}_j}^\mathrm{T}{V_\mu}^{-1}(\mathbf{E_T}_\mu + \mathbf{\Phi}_k\mathcal{E}_k) + \frac{\delta_{jk}\mathcal{E}_k}{V_\mathrm{tower}} - \frac{{\langle E_T \rangle}_\mathrm{tower}}{V_\mathrm{tower}} \\
  0 = (s{\mathbf{\Phi}_j}^\mathrm{T}{V_\mu}^{-1}\mathbf{\Phi}_k + \frac{\delta_{jk}}{V_\mathrm{tower}})\mathcal{E}_k + s{\mathbf{\Phi}_j}^\mathrm{T}{V_\mu}^{-1}\mathbf{E_T}_\mu - \frac{{\langle E_T \rangle}_\mathrm{tower}}{V_{tower}}.
```
The minimisation solution can therefore be written as the matrix equation

```math
  X_{ij}\mathcal{E}_j = c_i \\
  X_{ij} = s{\mathbf{\Phi}_i}^\mathrm{T}{V_\mu}^{-1}\mathbf{\Phi}_j + \frac{\delta_{ij}}{V_\mathrm{tower}} \\
  c_i    = \frac{{\langle E_T \rangle}_\mathrm{tower}}{V_\mathrm{tower}} - s{\mathbf{\Phi}_j}^\mathrm{T}{V_\mu}^{-1}\mathbf{E_T}_\mu \\
  \mathcal{E}_j = X_{ij}^{-1} c_{i} 
```
These corrections can then be applied to the original hard-scatter objects

------------
Code details
------------

The main pufit correction is done by the `pufit` function in
[../TrigEFMissingET/PufitUtils.h](../TrigEFMissingET/PufitUtils.h). This takes as inputs the sum over pileup
objects and the corresponding covariance matrix, the mean and variance estimates
for the towers, the directions of the hard scatter objects and the relative
importances to assign to the fit constraints ($`s`$).

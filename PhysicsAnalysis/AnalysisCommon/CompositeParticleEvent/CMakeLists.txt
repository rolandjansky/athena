################################################################################
# Package: CompositeParticleEvent
################################################################################

# Declare the package name:
atlas_subdir( CompositeParticleEvent )

atlas_add_library( CompositeParticleEvent
                   CompositeParticleEvent/*.h
                   INTERFACE
                   PUBLIC_HEADERS CompositeParticleEvent
                   LINK_LIBRARIES ParticleEvent )


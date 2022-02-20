/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SITRACKEREVENT_SITRACKDISTANCE_H
#define SITRACKEREVENT_SITRACKDISTANCE_H

/**
 * @brief Simple class that holds informattion about the angles (phi and theta)
 * and distance (in x and y) to a truth track wrt a diode (pixel)
 * 
 */

class SiTrackDistance {

public:

    /**
     * @brief Construct a new Si Track Distance object with the default parameters
     * 
     */
    SiTrackDistance();

    /**
     * @brief Construct a new Si Track Distance object
     * 
     * @param tanTheta tangent of the theta angle of the track 
     * @param tanPhi tangent of the phi angle of the track
     * @param zInit z position of the charge entrance
     */
    SiTrackDistance(float tanTheta, float tanPhi, float zInit);

    /**
     * @brief Destroy the Si Track Distance object
     * 
     */
    ~SiTrackDistance() = default;

    /**
     * @brief Default copy/move/assignment
     * 
     */
    SiTrackDistance(const SiTrackDistance&) = default; 
    SiTrackDistance& operator=(const SiTrackDistance&) = default; 
    SiTrackDistance(SiTrackDistance&&) = default; 
    SiTrackDistance& operator=(SiTrackDistance&&) = default; 

    /**
     * @brief Get the tangent of the Theta angle of the track
     * 
     * @return float 
     */
    inline float tanTheta() const {return m_tanTheta;}
    
    /**
     * @brief Get the tangent Phi angle of the track
     * 
     * @return float 
     */
    inline float tanPhi() const {return m_tanPhi;}
    
    /**
     * @brief Get the initial Z position of the track
     * 
     * @return float 
     */
    inline float zInit() const {return m_zInit;}
    
private:

    /**
     * @brief tangent of the angle theta of the track
     * 
     */
    float m_tanTheta{};

    /**
     * @brief tangent of the angle phi of the track
     * 
     */
    float m_tanPhi{};

    /**
     * @brief Initial Z position to the track
     * 
     */
    float m_zInit{};
};

#endif

/*
 Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
 */




#include <xAODPrimitives/IsolationHelpers.h>
#include <IsolationSelection/IsoVariableHelper.h>
#include <xAODBase/IParticleHelpers.h>
namespace CP {
    
    //######################################################################################################
    //                                      IsoVariableHelper
    //######################################################################################################
    IsoVariableHelper::IsoVariableHelper(xAOD::Iso::IsolationType type, const std::string& BackupPreFix) :
                m_isoType(type),
                m_BackupIso(!BackupPreFix.empty()),
                m_dec_IsoIsBackup("IsBackup_" + std::string(xAOD::Iso::toString(type)) + (BackupPreFix.empty() ? "" : "_") + BackupPreFix),
                m_acc_IsoIsBackup("IsBackup_" + std::string(xAOD::Iso::toString(type)) + (BackupPreFix.empty() ? "" : "_") + BackupPreFix),
                m_acc_iso_variable(xAOD::Iso::toString(type)),
                m_dec_iso_variable(xAOD::Iso::toString(type)),
                m_acc_iso_backup(std::string(xAOD::Iso::toString(type)) + (BackupPreFix.empty() ? "" : "_") + BackupPreFix),
                m_dec_iso_backup(std::string(xAOD::Iso::toString(type)) + (BackupPreFix.empty() ? "" : "_") + BackupPreFix) {
    }

    CorrectionCode IsoVariableHelper::getOrignalIsolation(const xAOD::IParticle* particle, float& value) const {
        if (!particle) {
            Error("IsoVariableHelper::GetOrignalIsolation()", "No particle given");
            return CorrectionCode::Error;
        }
        if (!m_BackupIso) {
            return getIsolationFromOriginal(particle,value);
        } else {
            if (!m_acc_IsoIsBackup.isAvailable(*particle) || !m_acc_IsoIsBackup(*particle)) {
                Warning("IsoVariableHelper::GetOrignalIsolation()", "No isolation value was backuped thus far. Did you call the BackupIsolation before for %s?", xAOD::Iso::toString(isotype()));
                return CorrectionCode::Error;
            } else {
                value = m_acc_iso_backup(*particle);
            }
        }
        return CorrectionCode::Ok;
    }
    CorrectionCode IsoVariableHelper::getIsolationFromOriginal(const xAOD::IParticle* particle, float& value) const{
        const xAOD::IParticle* originalParticle = xAOD::getOriginalObject(*particle);
        if (originalParticle && getIsolation(originalParticle, value) == CorrectionCode::Error) return CorrectionCode::Error;
        else if (!originalParticle) {
            Warning("IsoVariableHelper::GetOrignalIsolation()", "No original object was found");
            return getIsolation(particle, value);
        }    
        return CorrectionCode::Ok;
    }
    CorrectionCode IsoVariableHelper::getIsolation(const xAOD::IParticle* particle, float& value) const {
        if (!particle || !m_acc_iso_variable.isAvailable(*particle)) {
            Error("IsoVariableHelper::GetIsolation()", "Failed to retrieve isolation %s", xAOD::Iso::toString(isotype()));
            return CorrectionCode::Error;
        }
        value = m_acc_iso_variable(*particle);
        return CorrectionCode::Ok;
    }
    CorrectionCode IsoVariableHelper::backupIsolation(const xAOD::IParticle* particle) const {
        if (!particle) {
            Error("IsoVariableHelper::GetIsolation()", "No particle  given");
            return CorrectionCode::Error;
        }
        if (m_BackupIso && (!m_acc_IsoIsBackup.isAvailable(*particle) || !m_acc_IsoIsBackup(*particle))) {
            float Isovalue = 0;
            if (getIsolationFromOriginal(particle, Isovalue) == CorrectionCode::Error) {
                return CorrectionCode::Error;
            }
            m_dec_IsoIsBackup(*particle) = true;
            m_dec_iso_backup(*particle) = Isovalue;
        }
        return CorrectionCode::Ok;
    }
    CorrectionCode IsoVariableHelper::setIsolation(xAOD::IParticle* particle, float value) const {
        if (!particle) {
            Error("IsoVariableHelper::SetIsolation()", "No particle given");
            return CorrectionCode::Error;
        }
        if (std::isnan(value) || std::isinf(value)) {
            Error("IsoVariableHelper::SetIsolation()", "The value is not a number");
            return CorrectionCode::Error;
        }
        m_dec_iso_variable(*particle) = value;
        return CorrectionCode::Ok;
    }
    IsoType IsoVariableHelper::isotype() const {
        return m_isoType;
    }
    std::string IsoVariableHelper::name() const {
        return std::string(xAOD::Iso::toString(isotype()));
    }

}

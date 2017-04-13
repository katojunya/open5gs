#define TRACE_MODULE _esm_build

#include "core_debug.h"

#include "nas_message.h"

#include "nas_security.h"
#include "esm_build.h"

status_t esm_build_information_request(pkbuf_t **pkbuf, mme_esm_t *esm)
{
    nas_message_t message;
    mme_ue_t *ue = NULL;

    d_assert(esm, return CORE_ERROR, "Null param");
    ue = esm->ue;
    d_assert(ue, return CORE_ERROR, "Null param");

    memset(&message, 0, sizeof(message));
    message.h.security_header_type = 
       NAS_SECURITY_HEADER_INTEGRITY_PROTECTED_AND_CIPHERED;
    message.h.protocol_discriminator = NAS_PROTOCOL_DISCRIMINATOR_EMM;

    message.esm.h.protocol_discriminator = NAS_PROTOCOL_DISCRIMINATOR_ESM;
    message.esm.h.procedure_transaction_identity = esm->pti;
    message.esm.h.message_type = NAS_ESM_INFORMATION_REQUEST;

    d_assert(nas_security_encode(pkbuf, ue, &message) == CORE_OK && *pkbuf,,);

    return CORE_OK;
}

status_t esm_build_activate_default_bearer_context(
        pkbuf_t **pkbuf, mme_esm_t *esm)
{
    nas_message_t message;
    nas_activate_default_eps_bearer_context_request_t 
        *activate_default_eps_bearer_context_request = 
            &message.esm.activate_default_eps_bearer_context_request;
    nas_eps_quality_of_service_t *eps_qos =
        &activate_default_eps_bearer_context_request->eps_qos;
    nas_access_point_name_t *access_point_name =
        &activate_default_eps_bearer_context_request->access_point_name;
    nas_pdn_address_t *pdn_address = 
        &activate_default_eps_bearer_context_request->pdn_address;
    nas_apn_aggregate_maximum_bit_rate_t *apn_ambr =
        &activate_default_eps_bearer_context_request->apn_ambr;
    nas_protocol_configuration_options_t *protocol_configuration_options =
        &activate_default_eps_bearer_context_request
            ->protocol_configuration_options;
    
    mme_ue_t *ue = NULL;
    pdn_t *pdn = NULL;

    d_assert(esm, return CORE_ERROR, "Null param");
    ue = esm->ue;
    d_assert(ue, return CORE_ERROR, "Null param");
    pdn = esm->pdn;
    d_assert(pdn, return CORE_ERROR, "Null param");

    memset(&message, 0, sizeof(message));
    message.esm.h.eps_bearer_identity = esm->ebi;
    message.esm.h.protocol_discriminator = NAS_PROTOCOL_DISCRIMINATOR_ESM;
    message.esm.h.procedure_transaction_identity = esm->pti;
    message.esm.h.message_type = 
        NAS_ACTIVATE_DEFAULT_EPS_BEARER_CONTEXT_REQUEST;

    eps_qos->length = 5;
    eps_qos->qci = pdn->qci;
    eps_qos->ul_mbr = 0xff;
    eps_qos->dl_mbr = 0xff;
    eps_qos->ul_gbr = 0xff;
    eps_qos->dl_gbr = 0xff;

    access_point_name->length = strlen(pdn->apn);
    strcpy(access_point_name->apn, pdn->apn);

    pdn_address->length = PAA_IPV4_LEN;
    memcpy(&pdn_address->paa, &pdn->paa, pdn_address->length);

    activate_default_eps_bearer_context_request->presencemask |=
        NAS_ACTIVATE_DEFAULT_EPS_BEARER_CONTEXT_REQUEST_APN_AMBR_PRESENT;
    apn_ambr->length = 6;
    apn_ambr->dl_apn_ambr_extended2 = 4;
    apn_ambr->ul_apn_ambr_extended2 = 4;

    if (esm->pgw_pco_len)
    {
        activate_default_eps_bearer_context_request->presencemask |=
            NAS_ACTIVATE_DEFAULT_EPS_BEARER_CONTEXT_REQUEST_PROTOCOL_CONFIGURATION_OPTIONS_PRESENT;
        protocol_configuration_options->length = esm->pgw_pco_len;
        memcpy(protocol_configuration_options->buffer, 
                esm->pgw_pco, protocol_configuration_options->length);
    }

    d_assert(nas_plain_encode(pkbuf, &message) == CORE_OK && *pkbuf,,);

    return CORE_OK;
}

/* Copyright (c) 2008, 2017, Oracle and/or its affiliates. All rights reserved.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License, version 2.0,
   as published by the Free Software Foundation.

   This program is also distributed with certain software (including
   but not limited to OpenSSL) that is licensed under separate terms,
   as designated in a particular file or component or in included license
   documentation.  The authors of MySQL hereby grant you an additional
   permission to link the program and your derivative works with the
   separately licensed software that they have included with MySQL.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License, version 2.0, for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA */

#include "DblqhProxy.hpp"
#include "Dblqh.hpp"
#include "DblqhCommon.hpp"

#include <signaldata/StartFragReq.hpp>
#include <signaldata/ExecFragReq.hpp>
#include <signaldata/DumpStateOrd.hpp>

#define JAM_FILE_ID 442


DblqhProxy::DblqhProxy(Block_context& ctx) :
  LocalProxy(DBLQH, ctx),
  c_tableRecSize(0),
  c_tableRec(0)
{
  // GSN_CREATE_TAB_REQ
  addRecSignal(GSN_CREATE_TAB_REQ, &DblqhProxy::execCREATE_TAB_REQ);
  addRecSignal(GSN_CREATE_TAB_CONF, &DblqhProxy::execCREATE_TAB_CONF);
  addRecSignal(GSN_CREATE_TAB_REF, &DblqhProxy::execCREATE_TAB_REF);

  // GSN_LQHADDATTREQ
  addRecSignal(GSN_LQHADDATTREQ, &DblqhProxy::execLQHADDATTREQ);
  addRecSignal(GSN_LQHADDATTCONF, &DblqhProxy::execLQHADDATTCONF);
  addRecSignal(GSN_LQHADDATTREF, &DblqhProxy::execLQHADDATTREF);

  // GSN_LQHFRAGREQ
  addRecSignal(GSN_LQHFRAGREQ, &DblqhProxy::execLQHFRAGREQ);

  // GSN_TAB_COMMITREQ
  addRecSignal(GSN_TAB_COMMITREQ, &DblqhProxy::execTAB_COMMITREQ);
  addRecSignal(GSN_TAB_COMMITCONF, &DblqhProxy::execTAB_COMMITCONF);
  addRecSignal(GSN_TAB_COMMITREF, &DblqhProxy::execTAB_COMMITREF);

  // GSN_LCP_FRAG_ORD
  addRecSignal(GSN_LCP_FRAG_ORD, &DblqhProxy::execLCP_FRAG_ORD);
  addRecSignal(GSN_LCP_FRAG_REP, &DblqhProxy::execLCP_FRAG_REP);
  addRecSignal(GSN_END_LCP_CONF, &DblqhProxy::execEND_LCP_CONF);
  addRecSignal(GSN_LCP_COMPLETE_REP, &DblqhProxy::execLCP_COMPLETE_REP);

  addRecSignal(GSN_EMPTY_LCP_REQ, &DblqhProxy::execEMPTY_LCP_REQ);

  // GSN_GCP_SAVEREQ
  addRecSignal(GSN_GCP_SAVEREQ, &DblqhProxy::execGCP_SAVEREQ);
  addRecSignal(GSN_GCP_SAVECONF, &DblqhProxy::execGCP_SAVECONF);
  addRecSignal(GSN_GCP_SAVEREF, &DblqhProxy::execGCP_SAVEREF);

  // GSN_PREP_DROP_TAB_REQ
  addRecSignal(GSN_PREP_DROP_TAB_REQ, &DblqhProxy::execPREP_DROP_TAB_REQ);
  addRecSignal(GSN_PREP_DROP_TAB_CONF, &DblqhProxy::execPREP_DROP_TAB_CONF);
  addRecSignal(GSN_PREP_DROP_TAB_REF, &DblqhProxy::execPREP_DROP_TAB_REF);

  // GSN_DROP_TAB_REQ
  addRecSignal(GSN_DROP_TAB_REQ, &DblqhProxy::execDROP_TAB_REQ);
  addRecSignal(GSN_DROP_TAB_CONF, &DblqhProxy::execDROP_TAB_CONF);
  addRecSignal(GSN_DROP_TAB_REF, &DblqhProxy::execDROP_TAB_REF);

  // GSN_ALTER_TAB_REQ
  addRecSignal(GSN_ALTER_TAB_REQ, &DblqhProxy::execALTER_TAB_REQ);
  addRecSignal(GSN_ALTER_TAB_CONF, &DblqhProxy::execALTER_TAB_CONF);
  addRecSignal(GSN_ALTER_TAB_REF, &DblqhProxy::execALTER_TAB_REF);

  // GSN_START_FRAGREQ
  addRecSignal(GSN_START_FRAGREQ, &DblqhProxy::execSTART_FRAGREQ);

  // GSN_START_RECREQ
  addRecSignal(GSN_START_RECREQ, &DblqhProxy::execSTART_RECREQ);
  addRecSignal(GSN_START_RECCONF, &DblqhProxy::execSTART_RECCONF);

  // GSN_LQH_TRANSREQ
  addRecSignal(GSN_LQH_TRANSREQ, &DblqhProxy::execLQH_TRANSREQ);
  addRecSignal(GSN_LQH_TRANSCONF, &DblqhProxy::execLQH_TRANSCONF);

  // GSN_SUB_GCP_COMPLETE_REP
  addRecSignal(GSN_SUB_GCP_COMPLETE_REP, &DblqhProxy::execSUB_GCP_COMPLETE_REP);

  // GSN_EXEC_SRREQ
  addRecSignal(GSN_EXEC_SRREQ, &DblqhProxy::execEXEC_SRREQ);
  addRecSignal(GSN_EXEC_SRCONF, &DblqhProxy::execEXEC_SRCONF);

  // GSN_EXEC_FRAG
  addRecSignal(GSN_EXEC_FRAGREQ, &DblqhProxy::execEXEC_FRAGREQ);
  addRecSignal(GSN_EXEC_FRAGCONF, &DblqhProxy::execEXEC_FRAGCONF);

  // GSN_DROP_FRAG_REQ
  addRecSignal(GSN_DROP_FRAG_REQ, &DblqhProxy::execDROP_FRAG_REQ);
  addRecSignal(GSN_DROP_FRAG_CONF, &DblqhProxy::execDROP_FRAG_CONF);
  addRecSignal(GSN_DROP_FRAG_REF, &DblqhProxy::execDROP_FRAG_REF);

}

DblqhProxy::~DblqhProxy()
{
}

SimulatedBlock*
DblqhProxy::newWorker(Uint32 instanceNo)
{
  return new Dblqh(m_ctx, instanceNo);
}

// GSN_NDB_STTOR

void
DblqhProxy::callNDB_STTOR(Signal* signal)
{
  Ss_READ_NODES_REQ& ss = c_ss_READ_NODESREQ;
  ndbrequire(ss.m_gsn == 0);

  const Uint32 startPhase = signal->theData[2];
  switch (startPhase) {
  case 3:
    ss.m_gsn = GSN_NDB_STTOR;
    sendREAD_NODESREQ(signal);
    break;
  default:
    backNDB_STTOR(signal);
    break;
  }
}

// GSN_READ_CONFIG_REQ
void
DblqhProxy::callREAD_CONFIG_REQ(Signal* signal)
{
  const ReadConfigReq* req = (const ReadConfigReq*)signal->getDataPtr();
  ndbrequire(req->noOfParameters == 0);

  const ndb_mgm_configuration_iterator * p = 
    m_ctx.m_config.getOwnConfigIterator();
  ndbrequire(p != 0);
  
  ndbrequire(!ndb_mgm_get_int_parameter(p, CFG_TUP_TABLE, &c_tableRecSize));
  c_tableRec = (Uint8*)allocRecord("TableRec", sizeof(Uint8), c_tableRecSize);
  D("proxy:" << V(c_tableRecSize));
  Uint32 i;
  for (i = 0; i < c_tableRecSize; i++)
    c_tableRec[i] = 0;
  backREAD_CONFIG_REQ(signal);
}

// GSN_CREATE_TAB_REQ

// there is no consistent LQH connect pointer to use as ssId

void
DblqhProxy::execCREATE_TAB_REQ(Signal* signal)
{
  Ss_CREATE_TAB_REQ& ss = ssSeize<Ss_CREATE_TAB_REQ>(1);

  const CreateTabReq* req = (const CreateTabReq*)signal->getDataPtr();
  ss.m_req = *req;
  ndbrequire(signal->getLength() == CreateTabReq::SignalLengthLDM);

  sendREQ(signal, ss);
}

void
DblqhProxy::sendCREATE_TAB_REQ(Signal* signal, Uint32 ssId,
                               SectionHandle* handle)
{
  Ss_CREATE_TAB_REQ& ss = ssFind<Ss_CREATE_TAB_REQ>(ssId);

  CreateTabReq* req = (CreateTabReq*)signal->getDataPtrSend();
  *req = ss.m_req;
  req->senderRef = reference();
  req->senderData = ssId;
  sendSignalNoRelease(workerRef(ss.m_worker), GSN_CREATE_TAB_REQ,
                      signal, CreateTabReq::SignalLengthLDM, JBB, handle);
}

void
DblqhProxy::execCREATE_TAB_CONF(Signal* signal)
{
  const CreateTabConf* conf = (const CreateTabConf*)signal->getDataPtr();
  Uint32 ssId = conf->senderData;
  Ss_CREATE_TAB_REQ& ss = ssFind<Ss_CREATE_TAB_REQ>(ssId);
  recvCONF(signal, ss);
}

void
DblqhProxy::execCREATE_TAB_REF(Signal* signal)
{
  const CreateTabRef* ref = (const CreateTabRef*)signal->getDataPtr();
  Uint32 ssId = ref->senderData;
  Ss_CREATE_TAB_REQ& ss = ssFind<Ss_CREATE_TAB_REQ>(ssId);
  recvREF(signal, ss, ref->errorCode);
}

void
DblqhProxy::sendCREATE_TAB_CONF(Signal* signal, Uint32 ssId)
{
  Ss_CREATE_TAB_REQ& ss = ssFind<Ss_CREATE_TAB_REQ>(ssId);
  BlockReference dictRef = ss.m_req.senderRef;

  {
    const CreateTabConf* conf = (const CreateTabConf*)signal->getDataPtr();
    ss.m_lqhConnectPtr[ss.m_worker] = conf->lqhConnectPtr;
  }

  if (!lastReply(ss))
    return;

  if (ss.m_error == 0) {
    jam();
    CreateTabConf* conf = (CreateTabConf*)signal->getDataPtrSend();
    conf->senderRef = reference();
    conf->senderData = ss.m_req.senderData;
    conf->lqhConnectPtr = ssId;
    sendSignal(dictRef, GSN_CREATE_TAB_CONF,
               signal, CreateTabConf::SignalLength, JBB);

    // inform DBTUP proxy
    CreateTabReq* req = (CreateTabReq*)signal->getDataPtrSend();
    *req = ss.m_req;
    EXECUTE_DIRECT(DBTUP, GSN_CREATE_TAB_REQ,
                   signal, CreateTabReq::SignalLength);

    Uint32 tableId = ss.m_req.tableId;
    ndbrequire(tableId < c_tableRecSize);
    c_tableRec[tableId] = 1;
  } else {
    CreateTabRef* ref = (CreateTabRef*)signal->getDataPtrSend();
    ref->senderRef = reference();
    ref->senderData = ss.m_req.senderData;
    ref->errorCode = ss.m_error;
    ref->errorLine = 0;
    ref->errorKey = 0;
    ref->errorStatus = 0;
    sendSignal(dictRef, GSN_CREATE_TAB_REF,
               signal, CreateTabRef::SignalLength, JBB);
    ssRelease<Ss_CREATE_TAB_REQ>(ssId);
  }
}

// GSN_LQHADDATTREQ [ sub-op ]

void
DblqhProxy::execLQHADDATTREQ(Signal* signal)
{
  const LqhAddAttrReq* req = (const LqhAddAttrReq*)signal->getDataPtr();
  Uint32 ssId = req->lqhFragPtr;
  Ss_LQHADDATTREQ& ss = ssSeize<Ss_LQHADDATTREQ>(ssId);

  const Uint32 reqlength =
    LqhAddAttrReq::HeaderLength +
    req->noOfAttributes * LqhAddAttrReq::EntryLength;
  ndbrequire(signal->getLength() == reqlength);
  memcpy(&ss.m_req, req, reqlength << 2);
  ss.m_reqlength = reqlength;

  /**
   * Count LQHFRAGREQ, 
   *   so that I can release CREATE_TAB_REQ after last attribute has been
   *   processed
   */
  Ss_CREATE_TAB_REQ& ss_main = ssFind<Ss_CREATE_TAB_REQ>(ssId);
  ndbrequire(ss_main.m_req.noOfAttributes >= req->noOfAttributes);
  ss_main.m_req.noOfAttributes -= req->noOfAttributes;

  /* Save long section(s) in ss for forwarding to 
   * workers
   */
  SectionHandle handle(this, signal);
  saveSections(ss, handle);
  
  sendREQ(signal, ss);
}

void
DblqhProxy::sendLQHADDATTREQ(Signal* signal, Uint32 ssId, SectionHandle* handle)
{
  Ss_LQHADDATTREQ& ss = ssFind<Ss_LQHADDATTREQ>(ssId);
  Ss_CREATE_TAB_REQ& ss_main = ssFind<Ss_CREATE_TAB_REQ>(ssId);

  LqhAddAttrReq* req = (LqhAddAttrReq*)signal->getDataPtrSend();
  const Uint32 reqlength = ss.m_reqlength;
  memcpy(req, &ss.m_req, reqlength << 2);
  req->lqhFragPtr = ss_main.m_lqhConnectPtr[ss.m_worker];
  req->noOfAttributes = ss.m_req.noOfAttributes;
  req->senderData = ssId;
  req->senderAttrPtr = ss.m_req.senderAttrPtr;
  sendSignalNoRelease(workerRef(ss.m_worker), GSN_LQHADDATTREQ,
                      signal, reqlength, JBB, handle);
}

void
DblqhProxy::execLQHADDATTCONF(Signal* signal)
{
  const LqhAddAttrConf* conf = (const LqhAddAttrConf*)signal->getDataPtr();
  Uint32 ssId = conf->senderData;
  Ss_LQHADDATTREQ& ss = ssFind<Ss_LQHADDATTREQ>(ssId);
  recvCONF(signal, ss);
}

void
DblqhProxy::execLQHADDATTREF(Signal* signal)
{
  const LqhAddAttrRef* ref = (const LqhAddAttrRef*)signal->getDataPtr();
  Uint32 ssId = ref->senderData;
  Ss_LQHADDATTREQ& ss = ssFind<Ss_LQHADDATTREQ>(ssId);
  recvREF(signal, ss, ref->errorCode);
}

void
DblqhProxy::sendLQHADDATTCONF(Signal* signal, Uint32 ssId)
{
  Ss_LQHADDATTREQ& ss = ssFind<Ss_LQHADDATTREQ>(ssId);
  Ss_CREATE_TAB_REQ& ss_main = ssFind<Ss_CREATE_TAB_REQ>(ssId);
  BlockReference dictRef = ss_main.m_req.senderRef;

  if (!lastReply(ss))
    return;

  if (ss.m_error == 0) 
  {
    jam();
    LqhAddAttrConf* conf = (LqhAddAttrConf*)signal->getDataPtrSend();
    conf->senderData = ss.m_req.senderData;
    conf->senderAttrPtr = ss.m_req.senderAttrPtr;
    sendSignal(dictRef, GSN_LQHADDATTCONF,
               signal, LqhAddAttrConf::SignalLength, JBB);

    if (ss_main.m_req.noOfAttributes == 0)
    {
      jam();
      /**
       * All the attributes has been processed
       *   release create_table object
       */
      ssRelease<Ss_CREATE_TAB_REQ>(ssId);
    }
  }
  else 
  {
    jam();
    LqhAddAttrRef* ref = (LqhAddAttrRef*)signal->getDataPtrSend();
    ref->senderData = ss.m_req.senderData;
    ref->errorCode = ss.m_error;
    sendSignal(dictRef, GSN_LQHADDATTREF,
               signal, LqhAddAttrRef::SignalLength, JBB);
    ssRelease<Ss_CREATE_TAB_REQ>(ssId);
  }

  ssRelease<Ss_LQHADDATTREQ>(ssId);
}

// GSN_LQHFRAGREQ [ pass-through ]

void
DblqhProxy::execLQHFRAGREQ(Signal* signal)
{
  LqhFragReq* req = (LqhFragReq*)signal->getDataPtrSend();
  Uint32 instance = getInstanceKey(req->tableId, req->fragId);

  /* Ensure instance hasn't quietly mapped back to proxy! */
  ndbrequire(signal->getSendersBlockRef() != reference());

  // wl4391_todo impl. method that fakes senders block-ref
  sendSignal(numberToRef(DBLQH, instance, getOwnNodeId()),
             GSN_LQHFRAGREQ, signal, signal->getLength(), JBB);
}

// GSN_TAB_COMMITREQ [ sub-op ]

void
DblqhProxy::execTAB_COMMITREQ(Signal* signal)
{
  Ss_TAB_COMMITREQ& ss = ssSeize<Ss_TAB_COMMITREQ>(1); // lost connection

  const TabCommitReq* req = (const TabCommitReq*)signal->getDataPtr();
  ss.m_req = *req;
  sendREQ(signal, ss);
}

void
DblqhProxy::sendTAB_COMMITREQ(Signal* signal, Uint32 ssId,
                              SectionHandle* handle)
{
  Ss_TAB_COMMITREQ& ss = ssFind<Ss_TAB_COMMITREQ>(ssId);

  TabCommitReq* req = (TabCommitReq*)signal->getDataPtrSend();
  req->senderRef = reference();
  req->senderData = ssId;
  req->tableId = ss.m_req.tableId;
  sendSignalNoRelease(workerRef(ss.m_worker), GSN_TAB_COMMITREQ,
                      signal, TabCommitReq::SignalLength, JBB, handle);
}

void
DblqhProxy::execTAB_COMMITCONF(Signal* signal)
{
  const TabCommitConf* conf = (TabCommitConf*)signal->getDataPtr();
  Uint32 ssId = conf->senderData;
  Ss_TAB_COMMITREQ& ss = ssFind<Ss_TAB_COMMITREQ>(ssId);
  recvCONF(signal, ss);
}

void
DblqhProxy::execTAB_COMMITREF(Signal* signal)
{
  const TabCommitRef* ref = (TabCommitRef*)signal->getDataPtr();
  Uint32 ssId = ref->senderData;
  Ss_TAB_COMMITREQ& ss = ssFind<Ss_TAB_COMMITREQ>(ssId);

  // wl4391_todo omit extra info now since DBDICT only does ndbrequire
  recvREF(signal, ss, ref->errorCode);
}

void
DblqhProxy::sendTAB_COMMITCONF(Signal* signal, Uint32 ssId)
{
  Ss_TAB_COMMITREQ& ss = ssFind<Ss_TAB_COMMITREQ>(ssId);
  BlockReference dictRef = ss.m_req.senderRef;

  if (!lastReply(ss))
    return;

  if (ss.m_error == 0) {
    jam();
    TabCommitConf* conf = (TabCommitConf*)signal->getDataPtrSend();
    conf->senderData = ss.m_req.senderData;
    conf->nodeId = getOwnNodeId();
    conf->tableId = ss.m_req.tableId;
    sendSignal(dictRef, GSN_TAB_COMMITCONF,
               signal, TabCommitConf::SignalLength, JBB);
  } else {
    jam();
    TabCommitRef* ref = (TabCommitRef*)signal->getDataPtrSend();
    ref->senderData = ss.m_req.senderData;
    ref->nodeId = getOwnNodeId();
    ref->tableId = ss.m_req.tableId;
    sendSignal(dictRef, GSN_TAB_COMMITREF,
               signal, TabCommitRef::SignalLength, JBB);
    return;
  }

  ssRelease<Ss_TAB_COMMITREQ>(ssId);
}

// LCP handling

Uint32
DblqhProxy::getNoOfOutstanding(const LcpRecord & rec) const
{
  ndbrequire(rec.m_lcp_frag_ord_cnt >= rec.m_lcp_frag_rep_cnt);
  return rec.m_lcp_frag_ord_cnt - rec.m_lcp_frag_rep_cnt;
}

void
DblqhProxy::execLCP_FRAG_ORD(Signal* signal)
{
  ndbrequire(signal->getLength() == LcpFragOrd::SignalLength);

  const LcpFragOrd* req = (const LcpFragOrd*)signal->getDataPtr();
  const LcpFragOrd req_copy = *req;

  bool lcp_complete_ord = req->lastFragmentFlag;

  if (c_lcpRecord.m_state == LcpRecord::L_IDLE)
  {
    jam();
    D("LCP: start" << V(req->lcpId));
    c_lcpRecord.m_state = LcpRecord::L_STARTING;
    c_lcpRecord.m_lcpId = req->lcpId;
    c_lcpRecord.m_lcp_frag_rep_cnt = 0;
    c_lcpRecord.m_lcp_frag_ord_cnt = 0;
    c_lcpRecord.m_complete_outstanding = 0;
    c_lcpRecord.m_lastFragmentFlag = false;
    c_lcpRecord.m_empty_lcp_req.clear();

    // handle start of LCP in PGMAN and TSMAN
    LcpFragOrd* req = (LcpFragOrd*)signal->getDataPtrSend();
    *req = req_copy;
    EXECUTE_DIRECT(PGMAN, GSN_LCP_FRAG_ORD,
                   signal, LcpFragOrd::SignalLength);
    *req = req_copy;
    EXECUTE_DIRECT(TSMAN, GSN_LCP_FRAG_ORD,
                   signal, LcpFragOrd::SignalLength);

    c_lcpRecord.m_state = LcpRecord::L_RUNNING;
  }

  jam();
  D("LCP: continue" << V(req->lcpId) << V(c_lcpRecord.m_lcp_frag_ord_cnt));
  ndbrequire(c_lcpRecord.m_state == LcpRecord::L_RUNNING);
  ndbrequire(c_lcpRecord.m_lcpId == req->lcpId);

  if (lcp_complete_ord)
  {
    jam();
    c_lcpRecord.m_lastFragmentFlag = true;
    if (getNoOfOutstanding(c_lcpRecord) == 0)
    {
      jam();
      completeLCP_1(signal);
      return;
    }

    /**
     * Wait for all LCP_FRAG_ORD/REP to complete
     */
    return;
  }
  else
  {
    jam();
    c_lcpRecord.m_last_lcp_frag_ord = req_copy;
  }

  c_lcpRecord.m_lcp_frag_ord_cnt++;

  // Forward
  ndbrequire(req->tableId < c_tableRecSize);
  if (c_tableRec[req->tableId] == 0)
  {
    jam();
    /**
     * Send to lqh-0...that will handle it...
     */
    sendSignal(workerRef(0),
               GSN_LCP_FRAG_ORD, signal, LcpFragOrd::SignalLength, JBB);
  }
  else
  {
    jam();
    Uint32 instance = getInstanceKey(req->tableId, req->fragmentId);
    sendSignal(numberToRef(DBLQH, instance, getOwnNodeId()),
               GSN_LCP_FRAG_ORD, signal, LcpFragOrd::SignalLength, JBB);
  }
}

void
DblqhProxy::execLCP_FRAG_REP(Signal* signal)
{
  ndbrequire(signal->getLength() == LcpFragRep::SignalLength);

  LcpFragRep* conf = (LcpFragRep*)signal->getDataPtr();

  ndbrequire(c_lcpRecord.m_state == LcpRecord::L_RUNNING);
  ndbrequire(c_lcpRecord.m_lcpId == conf->lcpId);

  c_lcpRecord.m_lcp_frag_rep_cnt++;
  D("LCP: rep" << V(conf->lcpId) << V(c_lcpRecord.m_lcp_frag_rep_cnt));

  /**
   * But instead of broadcasting to all DIH's
   *   send to local that will do the broadcast
   */
  conf->nodeId = LcpFragRep::BROADCAST_REQ;
  sendSignal(DBDIH_REF, GSN_LCP_FRAG_REP,
             signal, LcpFragRep::SignalLength, JBB);

  if (c_lcpRecord.m_lastFragmentFlag)
  {
    jam();
    /**
     * lastFragmentFlag has arrived...
     */
    if (getNoOfOutstanding(c_lcpRecord) == 0)
    {
      jam();
      /*
       *   and we have all fragments has been processed
       */
      completeLCP_1(signal);
    }
    return;
  }

  checkSendEMPTY_LCP_CONF(signal);
}

void
DblqhProxy::completeLCP_1(Signal* signal)
{
  ndbrequire(c_lcpRecord.m_state == LcpRecord::L_RUNNING);
  c_lcpRecord.m_state = LcpRecord::L_COMPLETING_1;
  ndbrequire(c_lcpRecord.m_complete_outstanding == 0);

  /**
   * send LCP_FRAG_ORD (lastFragmentFlag = true)
   *   to all LQH instances...
   *   they will reply with LCP_COMPLETE_REP
   */
  LcpFragOrd* ord = (LcpFragOrd*)signal->getDataPtrSend();
  ord->lcpId = c_lcpRecord.m_lcpId;
  ord->lastFragmentFlag = true;
  for (Uint32 i = 0; i<c_workers; i++)
  {
    jam();
    c_lcpRecord.m_complete_outstanding++;
    sendSignal(workerRef(i), GSN_LCP_FRAG_ORD, signal,
               LcpFragOrd::SignalLength, JBB);
  }

  /**
   * send END_LCP_REQ to all pgman instances (except "extra" pgman)
   *   they will reply with END_LCP_CONF
   */
  EndLcpReq* req = (EndLcpReq*)signal->getDataPtrSend();
  req->senderData= 0;
  req->senderRef= reference();
  req->backupPtr= 0;
  req->backupId= c_lcpRecord.m_lcpId;
  for (Uint32 i = 0; i<c_workers; i++)
  {
    jam();
    c_lcpRecord.m_complete_outstanding++;
    sendSignal(numberToRef(PGMAN, workerInstance(i), getOwnNodeId()),
               GSN_END_LCP_REQ, signal, EndLcpReq::SignalLength, JBB);
  }
}

void
DblqhProxy::execLCP_COMPLETE_REP(Signal* signal)
{
  jamEntry();
  ndbrequire(c_lcpRecord.m_state == LcpRecord::L_COMPLETING_1);
  ndbrequire(c_lcpRecord.m_complete_outstanding);
  c_lcpRecord.m_complete_outstanding--;

  if (c_lcpRecord.m_complete_outstanding == 0)
  {
    jam();
    completeLCP_2(signal);
    return;
  }
}

void
DblqhProxy::execEND_LCP_CONF(Signal* signal)
{
  jamEntry();
  ndbrequire(c_lcpRecord.m_state == LcpRecord::L_COMPLETING_1 ||
             c_lcpRecord.m_state == LcpRecord::L_COMPLETING_2 ||
             c_lcpRecord.m_state == LcpRecord::L_COMPLETING_3);

  ndbrequire(c_lcpRecord.m_complete_outstanding);
  c_lcpRecord.m_complete_outstanding--;

  if (c_lcpRecord.m_complete_outstanding == 0)
  {
    jam();
    if (c_lcpRecord.m_state == LcpRecord::L_COMPLETING_1)
    {
      jam();
      completeLCP_2(signal);
      return;
    }
    else if (c_lcpRecord.m_state == LcpRecord::L_COMPLETING_2)
    {
      jam();
      completeLCP_3(signal);
      return;
    }
    else
    {
      jam();
      sendLCP_COMPLETE_REP(signal);
      return;
    }
  }
}

void
DblqhProxy::completeLCP_2(Signal* signal)
{
  jamEntry();
  ndbrequire(c_lcpRecord.m_state == LcpRecord::L_COMPLETING_1);
  c_lcpRecord.m_state = LcpRecord::L_COMPLETING_2;

  EndLcpReq* req = (EndLcpReq*)signal->getDataPtrSend();
  req->senderData= 0;
  req->senderRef= reference();
  req->backupPtr= 0;
  req->backupId= c_lcpRecord.m_lcpId;
  c_lcpRecord.m_complete_outstanding++;

  /**
   * send to "extra" instance
   *   that will checkpoint extent-pages
   */
  // NOTE: ugly to use MaxLqhWorkers directly
  Uint32 instance = c_workers + 1;
  sendSignal(numberToRef(PGMAN, instance, getOwnNodeId()),
             GSN_END_LCP_REQ, signal, EndLcpReq::SignalLength, JBB);
}


void
DblqhProxy::completeLCP_3(Signal* signal)
{
  jamEntry();
  ndbrequire(c_lcpRecord.m_state == LcpRecord::L_COMPLETING_2);
  c_lcpRecord.m_state = LcpRecord::L_COMPLETING_3;

  /**
   * And finally also checkpoint UNDO LOG
   *   and inform TSMAN that checkpoint is "complete"
   */
  EndLcpReq* req = (EndLcpReq*)signal->getDataPtrSend();
  req->senderData= 0;
  req->senderRef= reference();
  req->backupPtr= 0;
  req->backupId= c_lcpRecord.m_lcpId;

  // no reply from this
  sendSignal(TSMAN_REF, GSN_END_LCP_REQ, signal,
             EndLcpReq::SignalLength, JBB);

  if (c_lcpRecord.m_lcp_frag_rep_cnt)
  {
    jam();
    c_lcpRecord.m_complete_outstanding++;
    sendSignal(LGMAN_REF, GSN_END_LCP_REQ, signal,
               EndLcpReq::SignalLength, JBB);
  }
  else
  {
    jam();
    /**
     * lgman does currently not like 0 fragments,
     *   cause then it does not get a LCP_FRAG_ORD
     *
     *   this should change so that it gets this first (style)
     */
    sendLCP_COMPLETE_REP(signal);
  }
}

void
DblqhProxy::sendLCP_COMPLETE_REP(Signal* signal)
{
  ndbrequire(c_lcpRecord.m_state == LcpRecord::L_COMPLETING_3);

  LcpCompleteRep* conf = (LcpCompleteRep*)signal->getDataPtrSend();
  conf->nodeId = LcpFragRep::BROADCAST_REQ;
  conf->blockNo = DBLQH;
  conf->lcpId = c_lcpRecord.m_lcpId;
  sendSignal(DBDIH_REF, GSN_LCP_COMPLETE_REP,
             signal, LcpCompleteRep::SignalLength, JBB);

  c_lcpRecord.m_state = LcpRecord::L_IDLE;
  checkSendEMPTY_LCP_CONF(signal);
}

void
DblqhProxy::execEMPTY_LCP_REQ(Signal* signal)
{
  jam();

  CRASH_INSERTION(5008);

  EmptyLcpReq * const req = (EmptyLcpReq*)&signal->theData[0];
  Uint32 nodeId = refToNode(req->senderRef);
  c_lcpRecord.m_empty_lcp_req.set(nodeId);
  checkSendEMPTY_LCP_CONF(signal);
}

void
DblqhProxy::checkSendEMPTY_LCP_CONF_impl(Signal* signal)
{
  ndbrequire(!c_lcpRecord.m_empty_lcp_req.isclear());
  
  EmptyLcpRep * rep = (EmptyLcpRep*)signal->getDataPtrSend();
  EmptyLcpConf * conf = (EmptyLcpConf*)rep->conf;

  switch(c_lcpRecord.m_state){
  case LcpRecord::L_IDLE:
    jam();
    conf->idle = true;
    break;
  case LcpRecord::L_STARTING:
    jam();
    return;
  case LcpRecord::L_RUNNING:{
    jam();
    if (getNoOfOutstanding(c_lcpRecord) == 0)
    {
      jam();
      /**
       * Given that we wait for all ongoing...
       *   we can simply return last LCP_FRAG_ORD sent to us
       */
      conf->tableId = c_lcpRecord.m_last_lcp_frag_ord.tableId;
      conf->fragmentId = c_lcpRecord.m_last_lcp_frag_ord.fragmentId;
      conf->lcpId = c_lcpRecord.m_last_lcp_frag_ord.lcpId;
      conf->lcpNo = c_lcpRecord.m_last_lcp_frag_ord.lcpNo;
      break;
    }
    return;
  }
  case LcpRecord::L_COMPLETING_1:
    jam();
  case LcpRecord::L_COMPLETING_2:
    jam();
  case LcpRecord::L_COMPLETING_3:
    jam();
    return;
  }

  conf->senderNodeId = getOwnNodeId();

  c_lcpRecord.m_empty_lcp_req.copyto(NdbNodeBitmask::Size, rep->receiverGroup);
  sendSignal(DBDIH_REF, GSN_EMPTY_LCP_REP, signal,
             EmptyLcpRep::SignalLength + EmptyLcpConf::SignalLength, JBB);

  c_lcpRecord.m_empty_lcp_req.clear();
}

// GSN_GCP_SAVEREQ

void
DblqhProxy::execGCP_SAVEREQ(Signal* signal)
{
  const GCPSaveReq* req = (const GCPSaveReq*)signal->getDataPtr();
  Uint32 ssId = getSsId(req);
  Ss_GCP_SAVEREQ& ss = ssSeize<Ss_GCP_SAVEREQ>(ssId);
  ss.m_req = *req;
  sendREQ(signal, ss);
}

void
DblqhProxy::sendGCP_SAVEREQ(Signal* signal, Uint32 ssId, SectionHandle* handle)
{
  Ss_GCP_SAVEREQ& ss = ssFind<Ss_GCP_SAVEREQ>(ssId);

  GCPSaveReq* req = (GCPSaveReq*)signal->getDataPtrSend();
  *req = ss.m_req;

  req->dihBlockRef = reference();
  req->dihPtr = ss.m_worker;
  sendSignalNoRelease(workerRef(ss.m_worker), GSN_GCP_SAVEREQ,
                      signal, GCPSaveReq::SignalLength, JBB, handle);
}

void
DblqhProxy::execGCP_SAVECONF(Signal* signal)
{
  const GCPSaveConf* conf = (const GCPSaveConf*)signal->getDataPtr();
  Uint32 ssId = getSsId(conf);
  Ss_GCP_SAVEREQ& ss = ssFind<Ss_GCP_SAVEREQ>(ssId);
  recvCONF(signal, ss);
}

void
DblqhProxy::execGCP_SAVEREF(Signal* signal)
{
  const GCPSaveRef* ref = (const GCPSaveRef*)signal->getDataPtr();
  Uint32 ssId = getSsId(ref);
  Ss_GCP_SAVEREQ& ss = ssFind<Ss_GCP_SAVEREQ>(ssId);

  if (ss.m_error != 0) {
    // wl4391_todo check
    ndbrequire(ss.m_error == ref->errorCode);
  }
  recvREF(signal, ss, ref->errorCode);
}

void
DblqhProxy::sendGCP_SAVECONF(Signal* signal, Uint32 ssId)
{
  Ss_GCP_SAVEREQ& ss = ssFind<Ss_GCP_SAVEREQ>(ssId);

  if (!lastReply(ss))
    return;

  if (ss.m_error == 0) {
    GCPSaveConf* conf = (GCPSaveConf*)signal->getDataPtrSend();
    conf->dihPtr = ss.m_req.dihPtr;
    conf->nodeId = getOwnNodeId();
    conf->gci = ss.m_req.gci;
    sendSignal(ss.m_req.dihBlockRef, GSN_GCP_SAVECONF,
               signal, GCPSaveConf::SignalLength, JBB);
  } else {
    jam();
    GCPSaveRef* ref = (GCPSaveRef*)signal->getDataPtrSend();
    ref->dihPtr = ss.m_req.dihPtr;
    ref->nodeId = getOwnNodeId();
    ref->gci = ss.m_req.gci;
    ref->errorCode = ss.m_error;
    sendSignal(ss.m_req.dihBlockRef, GSN_GCP_SAVEREF,
               signal, GCPSaveRef::SignalLength, JBB);
  }

  ssRelease<Ss_GCP_SAVEREQ>(ssId);
}

// GSN_SUB_GCP_COMPLETE_REP
void
DblqhProxy::execSUB_GCP_COMPLETE_REP(Signal* signal)
{
  jamEntry();
  for (Uint32 i = 0; i<c_workers; i++)
  {
    jam();
    sendSignal(workerRef(i), GSN_SUB_GCP_COMPLETE_REP, signal,
               signal->getLength(), JBB);
  }
}

// GSN_PREP_DROP_TAB_REQ

void
DblqhProxy::execPREP_DROP_TAB_REQ(Signal* signal)
{
  const PrepDropTabReq* req = (const PrepDropTabReq*)signal->getDataPtr();
  Uint32 ssId = getSsId(req);
  Ss_PREP_DROP_TAB_REQ& ss = ssSeize<Ss_PREP_DROP_TAB_REQ>(ssId);
  ss.m_req = *req;
  ndbrequire(signal->getLength() == PrepDropTabReq::SignalLength);
  sendREQ(signal, ss);
}

void
DblqhProxy::sendPREP_DROP_TAB_REQ(Signal* signal, Uint32 ssId,
                                  SectionHandle * handle)
{
  Ss_PREP_DROP_TAB_REQ& ss = ssFind<Ss_PREP_DROP_TAB_REQ>(ssId);

  PrepDropTabReq* req = (PrepDropTabReq*)signal->getDataPtrSend();
  *req = ss.m_req;
  req->senderRef = reference();
  req->senderData = ssId; // redundant since tableId is used
  sendSignalNoRelease(workerRef(ss.m_worker), GSN_PREP_DROP_TAB_REQ,
                      signal, PrepDropTabReq::SignalLength, JBB, handle);
}

void
DblqhProxy::execPREP_DROP_TAB_CONF(Signal* signal)
{
  const PrepDropTabConf* conf = (const PrepDropTabConf*)signal->getDataPtr();
  Uint32 ssId = getSsId(conf);
  Ss_PREP_DROP_TAB_REQ& ss = ssFind<Ss_PREP_DROP_TAB_REQ>(ssId);
  recvCONF(signal, ss);
}

void
DblqhProxy::execPREP_DROP_TAB_REF(Signal* signal)
{
  const PrepDropTabRef* ref = (const PrepDropTabRef*)signal->getDataPtr();
  Uint32 ssId = getSsId(ref);
  Ss_PREP_DROP_TAB_REQ& ss = ssFind<Ss_PREP_DROP_TAB_REQ>(ssId);
  recvREF(signal, ss, ref->errorCode);
}

void
DblqhProxy::sendPREP_DROP_TAB_CONF(Signal* signal, Uint32 ssId)
{
  Ss_PREP_DROP_TAB_REQ& ss = ssFind<Ss_PREP_DROP_TAB_REQ>(ssId);
  BlockReference dictRef = ss.m_req.senderRef;

  if (!lastReply(ss))
    return;

  if (ss.m_error == 0) {
    jam();
    PrepDropTabConf* conf = (PrepDropTabConf*)signal->getDataPtrSend();
    conf->senderRef = reference();
    conf->senderData = ss.m_req.senderData;
    conf->tableId = ss.m_req.tableId;
    sendSignal(dictRef, GSN_PREP_DROP_TAB_CONF,
               signal, PrepDropTabConf::SignalLength, JBB);
  } else {
    jam();
    PrepDropTabRef* ref = (PrepDropTabRef*)signal->getDataPtrSend();
    ref->senderRef = reference();
    ref->senderData = ss.m_req.senderData;
    ref->tableId = ss.m_req.tableId;
    ref->errorCode = ss.m_error;
    sendSignal(dictRef, GSN_PREP_DROP_TAB_REF,
               signal, PrepDropTabRef::SignalLength, JBB);
  }

  ssRelease<Ss_PREP_DROP_TAB_REQ>(ssId);
}

// GSN_DROP_TAB_REQ

void
DblqhProxy::execDROP_TAB_REQ(Signal* signal)
{
  const DropTabReq* req = (const DropTabReq*)signal->getDataPtr();
  Uint32 ssId = getSsId(req);
  Ss_DROP_TAB_REQ& ss = ssSeize<Ss_DROP_TAB_REQ>(ssId);
  ss.m_req = *req;
  ndbrequire(signal->getLength() == DropTabReq::SignalLength);
  sendREQ(signal, ss);

  Uint32 tableId = ss.m_req.tableId;
  ndbrequire(tableId < c_tableRecSize);
  c_tableRec[tableId] = 0;
}

void
DblqhProxy::sendDROP_TAB_REQ(Signal* signal, Uint32 ssId, SectionHandle* handle)
{
  Ss_DROP_TAB_REQ& ss = ssFind<Ss_DROP_TAB_REQ>(ssId);

  DropTabReq* req = (DropTabReq*)signal->getDataPtrSend();
  *req = ss.m_req;
  req->senderRef = reference();
  req->senderData = ssId; // redundant since tableId is used
  sendSignalNoRelease(workerRef(ss.m_worker), GSN_DROP_TAB_REQ,
                      signal, DropTabReq::SignalLength, JBB, handle);
}

void
DblqhProxy::execDROP_TAB_CONF(Signal* signal)
{
  const DropTabConf* conf = (const DropTabConf*)signal->getDataPtr();
  Uint32 ssId = getSsId(conf);
  Ss_DROP_TAB_REQ& ss = ssFind<Ss_DROP_TAB_REQ>(ssId);
  recvCONF(signal, ss);
}

void
DblqhProxy::execDROP_TAB_REF(Signal* signal)
{
  const DropTabRef* ref = (const DropTabRef*)signal->getDataPtr();
  Uint32 ssId = getSsId(ref);
  Ss_DROP_TAB_REQ& ss = ssFind<Ss_DROP_TAB_REQ>(ssId);
  recvREF(signal, ss, ref->errorCode);
}

void
DblqhProxy::sendDROP_TAB_CONF(Signal* signal, Uint32 ssId)
{
  Ss_DROP_TAB_REQ& ss = ssFind<Ss_DROP_TAB_REQ>(ssId);
  BlockReference dictRef = ss.m_req.senderRef;

  if (!lastReply(ss))
    return;

  if (ss.m_error == 0) {
    jam();
    DropTabConf* conf = (DropTabConf*)signal->getDataPtrSend();
    conf->senderRef = reference();
    conf->senderData = ss.m_req.senderData;
    conf->tableId = ss.m_req.tableId;
    sendSignal(dictRef, GSN_DROP_TAB_CONF,
               signal, DropTabConf::SignalLength, JBB);

    // inform DBTUP proxy
    DropTabReq* req = (DropTabReq*)signal->getDataPtrSend();
    *req = ss.m_req;
    EXECUTE_DIRECT(DBTUP, GSN_DROP_TAB_REQ,
                   signal, DropTabReq::SignalLength);
  } else {
    jam();
    DropTabRef* ref = (DropTabRef*)signal->getDataPtrSend();
    ref->senderRef = reference();
    ref->senderData = ss.m_req.senderData;
    ref->tableId = ss.m_req.tableId;
    ref->errorCode = ss.m_error;
    sendSignal(dictRef, GSN_DROP_TAB_REF,
               signal, DropTabConf::SignalLength, JBB);
  }

  ssRelease<Ss_DROP_TAB_REQ>(ssId);
}

// GSN_ALTER_TAB_REQ

void
DblqhProxy::execALTER_TAB_REQ(Signal* signal)
{
  jamEntry();
  if (!assembleFragments(signal))
  {
    jam();
    return;
  }
  const AlterTabReq* req = (const AlterTabReq*)signal->getDataPtr();
  Uint32 ssId = getSsId(req);
  Ss_ALTER_TAB_REQ& ss = ssSeize<Ss_ALTER_TAB_REQ>(ssId);
  ss.m_req = *req;
  ndbrequire(signal->getLength() == AlterTabReq::SignalLength);

  SectionHandle handle(this, signal);
  saveSections(ss, handle);

  sendREQ(signal, ss);
}

void
DblqhProxy::sendALTER_TAB_REQ(Signal* signal, Uint32 ssId,
                              SectionHandle* handle)
{
  Ss_ALTER_TAB_REQ& ss = ssFind<Ss_ALTER_TAB_REQ>(ssId);

  AlterTabReq* req = (AlterTabReq*)signal->getDataPtrSend();
  *req = ss.m_req;
  req->senderRef = reference();
  req->senderData = ssId;
  sendSignalNoRelease(workerRef(ss.m_worker), GSN_ALTER_TAB_REQ,
                      signal, AlterTabReq::SignalLength, JBB, handle);
}

void
DblqhProxy::execALTER_TAB_CONF(Signal* signal)
{
  const AlterTabConf* conf = (const AlterTabConf*)signal->getDataPtr();
  Uint32 ssId = getSsId(conf);
  Ss_ALTER_TAB_REQ& ss = ssFind<Ss_ALTER_TAB_REQ>(ssId);
  recvCONF(signal, ss);
}

void
DblqhProxy::execALTER_TAB_REF(Signal* signal)
{
  const AlterTabRef* ref = (const AlterTabRef*)signal->getDataPtr();
  Uint32 ssId = getSsId(ref);
  Ss_ALTER_TAB_REQ& ss = ssFind<Ss_ALTER_TAB_REQ>(ssId);
  recvREF(signal, ss, ref->errorCode);
}

void
DblqhProxy::sendALTER_TAB_CONF(Signal* signal, Uint32 ssId)
{
  Ss_ALTER_TAB_REQ& ss = ssFind<Ss_ALTER_TAB_REQ>(ssId);
  BlockReference dictRef = ss.m_req.senderRef;

  if (!lastReply(ss))
    return;

  if (ss.m_error == 0) {
    jam();
    AlterTabConf* conf = (AlterTabConf*)signal->getDataPtrSend();
    conf->senderRef = reference();
    conf->senderData = ss.m_req.senderData;
    sendSignal(dictRef, GSN_ALTER_TAB_CONF,
               signal, AlterTabConf::SignalLength, JBB);
  } else {
    jam();
    AlterTabRef* ref = (AlterTabRef*)signal->getDataPtrSend();
    ref->senderRef = reference();
    ref->senderData = ss.m_req.senderData;
    ref->errorCode = ss.m_error;
    sendSignal(dictRef, GSN_ALTER_TAB_REF,
               signal, AlterTabConf::SignalLength, JBB);
  }

  ssRelease<Ss_ALTER_TAB_REQ>(ssId);
}

// GSN_START_FRAGREQ

void
DblqhProxy::execSTART_FRAGREQ(Signal* signal)
{
  StartFragReq* req = (StartFragReq*)signal->getDataPtrSend();
  Uint32 instance = getInstanceKey(req->tableId, req->fragId);

  // wl4391_todo impl. method that fakes senders block-ref
  sendSignal(numberToRef(DBLQH, instance, getOwnNodeId()),
             GSN_START_FRAGREQ, signal, signal->getLength(), JBB);
}

// GSN_START_RECREQ

void
DblqhProxy::execSTART_RECREQ(Signal* signal)
{
  if (refToMain(signal->getSendersBlockRef()) == DBLQH) {
    jam();
    execSTART_RECREQ_2(signal);
    return;
  }

  const StartRecReq* req = (const StartRecReq*)signal->getDataPtr();
  Ss_START_RECREQ& ss = ssSeize<Ss_START_RECREQ>();
  ss.m_req = *req;

  // seize records for sub-ops
  Uint32 i;
  for (i = 0; i < ss.m_req2cnt; i++) {
    Ss_START_RECREQ_2::Req tmp;
    tmp.proxyBlockNo = ss.m_req2[i].m_blockNo;
    Uint32 ssId2 = getSsId(&tmp);
    Ss_START_RECREQ_2& ss2 = ssSeize<Ss_START_RECREQ_2>(ssId2);
    ss.m_req2[i].m_ssId = ssId2;

    // set wait-for bitmask in SsParallel
    setMask(ss2);
  }

  ndbrequire(signal->getLength() == StartRecReq::SignalLength);
  sendREQ(signal, ss);
}

void
DblqhProxy::sendSTART_RECREQ(Signal* signal, Uint32 ssId, SectionHandle* handle)
{
  Ss_START_RECREQ& ss = ssFind<Ss_START_RECREQ>(ssId);

  StartRecReq* req = (StartRecReq*)signal->getDataPtrSend();
  *req = ss.m_req;

  req->senderRef = reference();
  req->senderData = ssId;
  sendSignalNoRelease(workerRef(ss.m_worker), GSN_START_RECREQ,
                      signal, StartRecReq::SignalLength, JBB, handle);
}

void
DblqhProxy::execSTART_RECCONF(Signal* signal)
{
  const StartRecConf* conf = (const StartRecConf*)signal->getDataPtr();

  if (refToMain(signal->getSendersBlockRef()) != DBLQH) {
    jam();
    execSTART_RECCONF_2(signal);
    return;
  }

  Uint32 ssId = conf->senderData;
  Ss_START_RECREQ& ss = ssFind<Ss_START_RECREQ>(ssId);
  recvCONF(signal, ss);
}

void
DblqhProxy::sendSTART_RECCONF(Signal* signal, Uint32 ssId)
{
  Ss_START_RECREQ& ss = ssFind<Ss_START_RECREQ>(ssId);

  if (!lastReply(ss))
    return;

  if (ss.m_error == 0) {
    jam();

    /**
     * There should be no disk-ops in flight here...check it
     */
    signal->theData[0] = DumpStateOrd::LgmanCheckCallbacksClear;
    sendSignal(LGMAN_REF, GSN_DUMP_STATE_ORD, signal, 1, JBB);

    StartRecConf* conf = (StartRecConf*)signal->getDataPtrSend();
    conf->startingNodeId = getOwnNodeId();
    conf->senderData = ss.m_req.senderData;
    sendSignal(ss.m_req.senderRef, GSN_START_RECCONF,
               signal, StartRecConf::SignalLength, JBB);
  } else {
    ndbrequire(false);
  }

  {
    Uint32 i;
    for (i = 0; i < ss.m_req2cnt; i++) {
      jam();
      Uint32 ssId2 = ss.m_req2[i].m_ssId;
      ssRelease<Ss_START_RECREQ_2>(ssId2);
    }
  }
  ssRelease<Ss_START_RECREQ>(ssId);
}

// GSN_START_RECREQ_2 [ sub-op, fictional gsn ]

void
DblqhProxy::execSTART_RECREQ_2(Signal* signal)
{
  ndbrequire(signal->getLength() == Ss_START_RECREQ_2::Req::SignalLength);

  const Ss_START_RECREQ_2::Req* req =
    (const Ss_START_RECREQ_2::Req*)signal->getDataPtr();
  Uint32 ssId = getSsId(req);
  Ss_START_RECREQ_2& ss = ssFind<Ss_START_RECREQ_2>(ssId);

  // reversed roles
  recvCONF(signal, ss);
}

void
DblqhProxy::sendSTART_RECREQ_2(Signal* signal, Uint32 ssId)
{
  Ss_START_RECREQ_2& ss = ssFind<Ss_START_RECREQ_2>(ssId);

  const Ss_START_RECREQ_2::Req* req =
    (const Ss_START_RECREQ_2::Req*)signal->getDataPtr();

  if (firstReply(ss)) {
    ss.m_req = *req;
  } else {
    jam();
    /*
     * Fragments can be started from different lcpId's.  LGMAN must run
     * UNDO until lowest lcpId.  Each DBLQH instance computes the lowest
     * lcpId in START_FRAGREQ.  In MT case the proxy further computes
     * the lowest of the lcpId's from worker instances.
     */
    if (req->lcpId < ss.m_req.lcpId)
    {
      jam();
      ss.m_req.lcpId = req->lcpId;
    }
    ndbrequire(ss.m_req.proxyBlockNo == req->proxyBlockNo);
  }

  if (!lastReply(ss))
    return;

  {
    Ss_START_RECREQ_2::Req* req =
      (Ss_START_RECREQ_2::Req*)signal->getDataPtrSend();
    *req = ss.m_req;
    BlockReference ref = numberToRef(req->proxyBlockNo, getOwnNodeId());
    sendSignal(ref, GSN_START_RECREQ, signal,
               Ss_START_RECREQ_2::Req::SignalLength, JBB);
  }
}

void
DblqhProxy::execSTART_RECCONF_2(Signal* signal)
{
  ndbrequire(signal->getLength() == Ss_START_RECREQ_2::Conf::SignalLength);

  const Ss_START_RECREQ_2::Conf* conf =
    (const Ss_START_RECREQ_2::Conf*)signal->getDataPtr();
  Uint32 ssId = getSsId(conf);
  Ss_START_RECREQ_2& ss = ssFind<Ss_START_RECREQ_2>(ssId);
  ss.m_conf = *conf;

  // reversed roles
  sendREQ(signal, ss);
}

void
DblqhProxy::sendSTART_RECCONF_2(Signal* signal, Uint32 ssId,
                                SectionHandle* handle)
{
  Ss_START_RECREQ_2& ss = ssFind<Ss_START_RECREQ_2>(ssId);

  Ss_START_RECREQ_2::Conf* conf =
    (Ss_START_RECREQ_2::Conf*)signal->getDataPtrSend();
  *conf = ss.m_conf;
  sendSignalNoRelease(workerRef(ss.m_worker), GSN_START_RECCONF, signal,
                      Ss_START_RECREQ_2::Conf::SignalLength, JBB, handle);
}

// GSN_LQH_TRANSREQ

void
DblqhProxy::execLQH_TRANSREQ(Signal* signal)
{
  jamEntry();
  
  if (!checkNodeFailSequence(signal))
  {
    jam();
    return;
  }
  const LqhTransReq* req = (const LqhTransReq*)signal->getDataPtr();
  Ss_LQH_TRANSREQ& ss = ssSeize<Ss_LQH_TRANSREQ>();
  ss.m_maxInstanceId = 0;
  ss.m_req = *req;
  if (signal->getLength() < LqhTransReq::SignalLength)
  {
    /**
     * TC that performs take over doesn't suppport taking over one
     * TC instance at a time
     */
     ss.m_req.instanceId = RNIL;
  }
  ndbrequire(signal->getLength() <= LqhTransReq::SignalLength);
  sendREQ(signal, ss);

  /**
   * See if this is a "resend" (i.e multi TC failure)
   *   and if so, mark "old" record as invalid
   */
  Uint32 nodeId = ss.m_req.failedNodeId;
  for (Uint32 i = 0; i<NDB_ARRAY_SIZE(c_ss_LQH_TRANSREQ.m_pool); i++)
  {
    if (c_ss_LQH_TRANSREQ.m_pool[i].m_ssId != 0 &&
        c_ss_LQH_TRANSREQ.m_pool[i].m_ssId != ss.m_ssId &&
        c_ss_LQH_TRANSREQ.m_pool[i].m_req.failedNodeId == nodeId)
    {
      jam();
      c_ss_LQH_TRANSREQ.m_pool[i].m_valid = false;
    }
  }
}

void
DblqhProxy::sendLQH_TRANSREQ(Signal* signal, Uint32 ssId, SectionHandle* handle)
{
  Ss_LQH_TRANSREQ& ss = ssFind<Ss_LQH_TRANSREQ>(ssId);

  LqhTransReq* req = (LqhTransReq*)signal->getDataPtrSend();
  *req = ss.m_req;

  req->senderData = ssId;
  req->senderRef = reference();
  sendSignalNoRelease(workerRef(ss.m_worker), GSN_LQH_TRANSREQ,
                      signal, LqhTransReq::SignalLength, JBB, handle);
}

void
DblqhProxy::execLQH_TRANSCONF(Signal* signal)
{
  const LqhTransConf* conf = (const LqhTransConf*)signal->getDataPtr();
  Uint32 ssId = conf->tcRef;
  Ss_LQH_TRANSREQ& ss = ssFind<Ss_LQH_TRANSREQ>(ssId);
  ss.m_conf = *conf;

  BlockReference ref = signal->getSendersBlockRef();
  ndbrequire(refToMain(ref) == number());
  const Uint32 ino = refToInstance(ref);
  const Uint32 worker = workerIndex(ino);
  
  ndbrequire(ref == workerRef(worker));
  ndbrequire(worker < c_workers);
  
  if (ss.m_valid == false)
  {
    jam();
    /**
     * This is an in-flight signal to an old take-over "session"
     */
    if (ss.m_conf.operationStatus == LqhTransConf::LastTransConf)
    {
      jam();
      ndbrequire(ss.m_workerMask.get(worker));
      ss.m_workerMask.clear(worker);
      if (ss.m_workerMask.isclear())
      {
        jam();
        ssRelease<Ss_LQH_TRANSREQ>(ssId);
      }
    }
    return;
  }
  else if (ss.m_conf.operationStatus == LqhTransConf::LastTransConf)
  {
    jam();
    /**
     * When completing(LqhTransConf::LastTransConf) a LQH_TRANSREQ
     *   also check if one can release obsoleteded records
     *
     * This could have been done on each LQH_TRANSCONF, but there is no
     *   urgency, so it's ok todo only on LastTransConf
     */
    Uint32 nodeId = ss.m_req.failedNodeId;
    for (Uint32 i = 0; i<NDB_ARRAY_SIZE(c_ss_LQH_TRANSREQ.m_pool); i++)
    {
      if (c_ss_LQH_TRANSREQ.m_pool[i].m_ssId != 0 &&
          c_ss_LQH_TRANSREQ.m_pool[i].m_ssId != ssId &&
          c_ss_LQH_TRANSREQ.m_pool[i].m_req.failedNodeId == nodeId &&
          c_ss_LQH_TRANSREQ.m_pool[i].m_valid == false)
      {
        jam();
        if (c_ss_LQH_TRANSREQ.m_pool[i].m_workerMask.get(worker))
        {
          jam();
          c_ss_LQH_TRANSREQ.m_pool[i].m_workerMask.clear(worker);
          if (c_ss_LQH_TRANSREQ.m_pool[i].m_workerMask.isclear())
          {
            jam();
            ssRelease<Ss_LQH_TRANSREQ>(c_ss_LQH_TRANSREQ.m_pool[i].m_ssId);
          }
        }
      }
    }
  }

  recvCONF(signal, ss);
}

void
DblqhProxy::sendLQH_TRANSCONF(Signal* signal, Uint32 ssId)
{
  Ss_LQH_TRANSREQ& ss = ssFind<Ss_LQH_TRANSREQ>(ssId);

  if (ss.m_conf.operationStatus == LqhTransConf::LastTransConf) 
  {
    jam();

    /**
     * Maintain running max instance id, based on max seen 
     * in this pass, which is only sent in LastTransConf 
     * LQH_TRANSREQ from each instance.
     */
    if (ss.m_conf.maxInstanceId > ss.m_maxInstanceId)
    {
      ndbrequire(ss.m_conf.maxInstanceId < NDBMT_MAX_BLOCK_INSTANCES);
      ss.m_maxInstanceId = ss.m_conf.maxInstanceId;
    }
  }
  else
  {
    jam();
    /* Forward conf to the requesting TC, and wait for more */
    LqhTransConf* conf = (LqhTransConf*)signal->getDataPtrSend();
    *conf = ss.m_conf;
    conf->tcRef = ss.m_req.senderData;
    sendSignal(ss.m_req.senderRef, GSN_LQH_TRANSCONF,
               signal, LqhTransConf::SignalLength, JBB);

    // more replies from this worker
    skipConf(ss);
  }

  if (!lastReply(ss))
    return;

  if (ss.m_error == 0) {
    jam();
    LqhTransConf* conf = (LqhTransConf*)signal->getDataPtrSend();
    conf->tcRef = ss.m_req.senderData;
    conf->lqhNodeId = getOwnNodeId();
    conf->operationStatus = LqhTransConf::LastTransConf;
    conf->maxInstanceId = ss.m_maxInstanceId;
    sendSignal(ss.m_req.senderRef, GSN_LQH_TRANSCONF,
               signal, LqhTransConf::SignalLength, JBB);
  } else {
    ndbrequire(false);
  }

  ssRelease<Ss_LQH_TRANSREQ>(ssId);
}

// GSN_EXEC_SR_1 [fictional gsn ]

void
DblqhProxy::execEXEC_SRREQ(Signal* signal)
{
  const BlockReference senderRef = signal->getSendersBlockRef();

  if (refToInstance(senderRef) != 0) {
    jam();
    execEXEC_SR_2(signal, GSN_EXEC_SRREQ);
    return;
  }

  execEXEC_SR_1(signal, GSN_EXEC_SRREQ);
}

void
DblqhProxy::execEXEC_SRCONF(Signal* signal)
{
  const BlockReference senderRef = signal->getSendersBlockRef();

  if (refToInstance(senderRef) != 0) {
    jam();
    execEXEC_SR_2(signal, GSN_EXEC_SRCONF);
    return;
  }

  execEXEC_SR_1(signal, GSN_EXEC_SRCONF);
}

void
DblqhProxy::execEXEC_SR_1(Signal* signal, GlobalSignalNumber gsn)
{
  ndbrequire(signal->getLength() == Ss_EXEC_SR_1::Sig::SignalLength);

  const Ss_EXEC_SR_1::Sig* sig =
    (const Ss_EXEC_SR_1::Sig*)signal->getDataPtr();
  Uint32 ssId = getSsId(sig);
  Ss_EXEC_SR_1& ss = ssSeize<Ss_EXEC_SR_1>(ssId);
  ss.m_gsn = gsn;
  ss.m_sig = *sig;

  sendREQ(signal, ss);
  ssRelease<Ss_EXEC_SR_1>(ss);
}

void
DblqhProxy::sendEXEC_SR_1(Signal* signal, Uint32 ssId, SectionHandle* handle)
{
  Ss_EXEC_SR_1& ss = ssFind<Ss_EXEC_SR_1>(ssId);
  signal->theData[0] = ss.m_sig.nodeId;
  sendSignalNoRelease(workerRef(ss.m_worker), ss.m_gsn, signal, 1, JBB, handle);
}

// GSN_EXEC_SRREQ_2 [ fictional gsn ]

void
DblqhProxy::execEXEC_SR_2(Signal* signal, GlobalSignalNumber gsn)
{
  ndbrequire(signal->getLength() == Ss_EXEC_SR_2::Sig::SignalLength);

  const Ss_EXEC_SR_2::Sig* sig =
    (const Ss_EXEC_SR_2::Sig*)signal->getDataPtr();
  Uint32 ssId = getSsId(sig);

  bool found = false;
  Ss_EXEC_SR_2& ss = ssFindSeize<Ss_EXEC_SR_2>(ssId, &found);
  if (!found) {
    jam();
    setMask(ss);
  }

  ndbrequire(sig->nodeId == getOwnNodeId());
  if (ss.m_sigcount == 0) {
    jam();
    ss.m_gsn = gsn;
    ss.m_sig = *sig;
  } else {
    jam();
    ndbrequire(ss.m_gsn == gsn);
    ndbrequire(memcmp(&ss.m_sig, sig, sizeof(*sig)) == 0);
  }
  ss.m_sigcount++;

  // reversed roles
  recvCONF(signal, ss);
}

void
DblqhProxy::sendEXEC_SR_2(Signal* signal, Uint32 ssId)
{
  Ss_EXEC_SR_2& ss = ssFind<Ss_EXEC_SR_2>(ssId);

  if (!lastReply(ss)) {
    jam();
    return;
  }

  NodeBitmask nodes;
  nodes.assign(NdbNodeBitmask::Size, ss.m_sig.sr_nodes);
  NodeReceiverGroup rg(DBLQH, nodes);

  signal->theData[0] = ss.m_sig.nodeId;
  sendSignal(rg, ss.m_gsn, signal, 1, JBB);

  ssRelease<Ss_EXEC_SR_2>(ssId);
}

// GSN_EXEC_FRAGREQ
void
DblqhProxy::execEXEC_FRAGREQ(Signal* signal)
{
  Uint32 ref = ((ExecFragReq*)signal->getDataPtr())->dst;

  if (refToNode(ref) == getOwnNodeId())
  {
    jam();
    sendSignal(ref, GSN_EXEC_FRAGREQ, signal, signal->getLength(), JBB);
  }
  else if (ndb_route_exec_frag(getNodeInfo(refToNode(ref)).m_version))
  {
    jam();
    sendSignal(numberToRef(DBLQH, refToNode(ref)), GSN_EXEC_FRAGREQ, signal,
               signal->getLength(), JBB);
  }
  else
  {
    jam();
    sendSignal(ref, GSN_EXEC_FRAGREQ, signal,
               signal->getLength(), JBB);
  }
}

// GSN_EXEC_FRAGCONF
void
DblqhProxy::execEXEC_FRAGCONF(Signal* signal)
{
  Uint32 ref = signal->theData[1];

  if (refToNode(ref) == getOwnNodeId())
  {
    jam();
    sendSignal(ref, GSN_EXEC_FRAGCONF, signal, 1, JBB);
  }
  else if (ndb_route_exec_frag(getNodeInfo(refToNode(ref)).m_version))
  {
    jam();
    sendSignal(numberToRef(DBLQH, refToNode(ref)), GSN_EXEC_FRAGCONF,
               signal, 2, JBB);
  }
  else
  {
    jam();
    sendSignal(ref, GSN_EXEC_FRAGCONF, signal, 2, JBB);
  }
}

// GSN_DROP_FRAG_REQ

void
DblqhProxy::execDROP_FRAG_REQ(Signal* signal)
{
  const DropFragReq* req = (const DropFragReq*)signal->getDataPtr();
  Uint32 ssId = getSsId(req);
  Ss_DROP_FRAG_REQ& ss = ssSeize<Ss_DROP_FRAG_REQ>(ssId);
  ss.m_req = *req;
  ndbrequire(signal->getLength() == DropFragReq::SignalLength);
  sendREQ(signal, ss);
}

void
DblqhProxy::sendDROP_FRAG_REQ(Signal* signal, Uint32 ssId,
                              SectionHandle* handle)
{
  Ss_DROP_FRAG_REQ& ss = ssFind<Ss_DROP_FRAG_REQ>(ssId);

  DropFragReq* req = (DropFragReq*)signal->getDataPtrSend();
  *req = ss.m_req;
  req->senderRef = reference();
  req->senderData = ssId;
  sendSignalNoRelease(workerRef(ss.m_worker), GSN_DROP_FRAG_REQ,
                      signal, DropFragReq::SignalLength, JBB, handle);
}

void
DblqhProxy::execDROP_FRAG_CONF(Signal* signal)
{
  const DropFragConf* conf = (const DropFragConf*)signal->getDataPtr();
  Uint32 ssId = getSsId(conf);
  Ss_DROP_FRAG_REQ& ss = ssFind<Ss_DROP_FRAG_REQ>(ssId);
  recvCONF(signal, ss);
}

void
DblqhProxy::execDROP_FRAG_REF(Signal* signal)
{
  const DropFragRef* ref = (const DropFragRef*)signal->getDataPtr();
  Uint32 ssId = getSsId(ref);
  Ss_DROP_FRAG_REQ& ss = ssFind<Ss_DROP_FRAG_REQ>(ssId);
  recvREF(signal, ss, ref->errCode);
}

void
DblqhProxy::sendDROP_FRAG_CONF(Signal* signal, Uint32 ssId)
{
  Ss_DROP_FRAG_REQ& ss = ssFind<Ss_DROP_FRAG_REQ>(ssId);
  BlockReference dictRef = ss.m_req.senderRef;

  if (!lastReply(ss))
    return;

  if (ss.m_error == 0) {
    jam();
    DropFragConf* conf = (DropFragConf*)signal->getDataPtrSend();
    conf->senderRef = reference();
    conf->senderData = ss.m_req.senderData;
    conf->tableId = ss.m_req.tableId;
    conf->fragId = ss.m_req.fragId;
    sendSignal(dictRef, GSN_DROP_FRAG_CONF,
               signal, DropFragConf::SignalLength, JBB);
  } else {
    jam();
    DropFragRef* ref = (DropFragRef*)signal->getDataPtrSend();
    ref->senderRef = reference();
    ref->senderData = ss.m_req.senderData;
    ref->tableId = ss.m_req.tableId;
    ref->fragId = ss.m_req.fragId;
    ref->errCode = ss.m_error;
    sendSignal(dictRef, GSN_DROP_FRAG_REF,
               signal, DropFragConf::SignalLength, JBB);
  }

  ssRelease<Ss_DROP_FRAG_REQ>(ssId);
}

BLOCK_FUNCTIONS(DblqhProxy)



/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 8.00.0603 */
/* at Tue Feb 03 16:32:26 2015
 */
/* Compiler settings for ShowPixels.idl:
    Oicf, W1, Zp8, env=Win32 (32b run), target_arch=X86 8.00.0603 
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
/* @@MIDL_FILE_HEADING(  ) */

#pragma warning( disable: 4049 )  /* more than 64k source lines */


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 475
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__


#ifndef __ShowPixels_h_h__
#define __ShowPixels_h_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IShowPixels_FWD_DEFINED__
#define __IShowPixels_FWD_DEFINED__
typedef interface IShowPixels IShowPixels;

#endif 	/* __IShowPixels_FWD_DEFINED__ */


#ifndef __ShowPixels_FWD_DEFINED__
#define __ShowPixels_FWD_DEFINED__

#ifdef __cplusplus
typedef class ShowPixels ShowPixels;
#else
typedef struct ShowPixels ShowPixels;
#endif /* __cplusplus */

#endif 	/* __ShowPixels_FWD_DEFINED__ */


#ifdef __cplusplus
extern "C"{
#endif 



#ifndef __ShowPixels_LIBRARY_DEFINED__
#define __ShowPixels_LIBRARY_DEFINED__

/* library ShowPixels */
/* [version][uuid] */ 


EXTERN_C const IID LIBID_ShowPixels;

#ifndef __IShowPixels_DISPINTERFACE_DEFINED__
#define __IShowPixels_DISPINTERFACE_DEFINED__

/* dispinterface IShowPixels */
/* [uuid] */ 


EXTERN_C const IID DIID_IShowPixels;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("CE078F46-E647-4EA4-829E-47B59EFAB6F4")
    IShowPixels : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct IShowPixelsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IShowPixels * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IShowPixels * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IShowPixels * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IShowPixels * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IShowPixels * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IShowPixels * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IShowPixels * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        END_INTERFACE
    } IShowPixelsVtbl;

    interface IShowPixels
    {
        CONST_VTBL struct IShowPixelsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IShowPixels_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IShowPixels_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IShowPixels_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IShowPixels_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IShowPixels_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IShowPixels_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IShowPixels_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* __IShowPixels_DISPINTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_ShowPixels;

#ifdef __cplusplus

class DECLSPEC_UUID("319858CF-0C76-40DC-A9D6-8E1A1C542E7F")
ShowPixels;
#endif
#endif /* __ShowPixels_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif



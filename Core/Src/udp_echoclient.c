/**
  ******************************************************************************
  * @file    LwIP/LwIP_UDP_Echo_Client/Src/udp_echoclient.c
  * @author  MCD Application Team
  * @brief   UDP echo client
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2017 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "lwip/pbuf.h"
#include "lwip/udp.h"
#include "lwip/tcp.h"
#include <string.h>
#include <stdio.h>
#include "udp_echoclient.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
void udp_receive_callback(void *arg, struct udp_pcb *upcb, struct pbuf *p, const ip_addr_t *addr, u16_t port);

u8_t   data[100];
__IO uint32_t message_count = 0;
__IO uint32_t message_count_tx = 0;
__IO uint32_t message_count_tx_error = 0;
ip_addr_t DestIPaddr;

char recive_buff[64];
struct udp_pcb *upcb;
err_t err;


/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Connect to UDP echo server
  * @param  None
  * @retval None
  */
void udp_echoclient_connect(void)
{
  //ip_addr_t DestIPaddr;
  err_t err;

  /* Create a new UDP control block  */

  upcb = udp_new();


  if (upcb!=NULL)
  {
    /*assign destination IP address */
	IP4_ADDR( &DestIPaddr, DEST_IP_ADDR0, DEST_IP_ADDR1, DEST_IP_ADDR2, DEST_IP_ADDR3 );

	//err = udp_connect(upcb, &DestIPaddr, UDP_SERVER_PORT);
	err = udp_bind(upcb, IP_ADDR_ANY, UDP_SERVER_PORT);
    /* configure destination IP address and port */


    if (err == ERR_OK)
    {
      /* Set a receive callback for the upcb */
      udp_recv(upcb, udp_receive_callback, NULL);
    }

  }
}

/**
  * @brief This function is called when an UDP datagrm has been received on the port UDP_PORT.
  * @param arg user supplied argument (udp_pcb.recv_arg)
  * @param pcb the udp_pcb which received data
  * @param p the packet buffer that was received
  * @param addr the remote IP address from which the packet was received
  * @param port the remote port from which the packet was received
  * @retval None
  */
void udp_echoclient_send(void)
{
  struct pbuf *p;

  sprintf((char*)data, "sending udp client message %d", (int)message_count);

  /* allocate pbuf from pool*/
  p = pbuf_alloc(PBUF_TRANSPORT,strlen((char*)data), PBUF_POOL);

  if (p != NULL)
  {
    /* copy data to pbuf */
    pbuf_take(p, (char*)data, strlen((char*)data));

    /* send udp data */
    //udp_send(upcb, p);
    udp_sendto(upcb, p,&DestIPaddr, UDP_CLIENT_PORT);

    /* free pbuf */
    pbuf_free(p);
    message_count_tx++;
  }
  else
  {
	  message_count_tx_error++;
  }
}

void ledToggle(char d)
{
	if(d>50) HAL_GPIO_WritePin(LD1_GPIO_Port, LD1_Pin, GPIO_PIN_SET);
	else HAL_GPIO_WritePin(LD1_GPIO_Port, LD1_Pin, GPIO_PIN_RESET);
}

void my_send_udp(char* ptr, int len)
{
	struct	pbuf *p;
	p = pbuf_alloc(PBUF_TRANSPORT, len, PBUF_POOL);
	if(p!=NULL)
	{
		/* copy data to pbuf */
		pbuf_take(p, (char*)ptr, len);
		//
	    /* send udp data */
	    udp_sendto(upcb, p,&DestIPaddr, UDP_CLIENT_PORT);

	    message_count_tx++;
		/* free pbuf */
		pbuf_free(p);
	}
}

/**
  * @brief This function is called when an UDP datagrm has been received on the port UDP_PORT.
  * @param arg user supplied argument (udp_pcb.recv_arg)
  * @param pcb the udp_pcb which received data
  * @param p the packet buffer that was received
  * @param addr the remote IP address from which the packet was received
  * @param port the remote port from which the packet was received
  * @retval None
  */
void udp_receive_callback(void *arg, struct udp_pcb *upcb, struct pbuf *p, const ip_addr_t *addr, u16_t port)
{
  /*increment message count */
  message_count++;
  strncpy(recive_buff, (char*) p->payload, p->len);
  int size = p->len;

  if (size>0) {  ledToggle(recive_buff[0]); }

  my_send_udp(recive_buff, size);
  /* Free receive pbuf */
  pbuf_free(p);
}

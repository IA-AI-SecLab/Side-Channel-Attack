% [ 파형 파일 생성 ] : Cutting + Y축 이동 
function z = read_wave6(FN1,FN2,num) % C1--new , C2-new , 개수 

st = 2;  % 시작 파형 인덱스 ( 0번째 = trace(:,1) )
name = "sigmoid2"; % Setting % 

fid = fopen(name+".bin", 'w'); % 파일 없어도 자동으로 생성


for i= st: st + num - 1 
    index = sprintf("%05d",i-1);
    trace = read_lecroywave("./" + name + "/"+FN1+"--"+index+".trc",i-1);  % 파일명 % 
    trigger = read_lecroywave("./" + name + "/"+FN2+"--"+index+".trc",i-1);  % 파일명 %
    
    
    % trigger 처리 
    for j=1 : length(trigger.y)
        if trigger.y(j)  > 3.2    % Setting %
           start = j; 
           fprintf("start : %d \n",start);
           break
        end
    end
    
    for j=length(trigger.y) : -1 : 1
        if trigger.y(j)  < -0.3   % Setting %
            end_ = j; 
            fprintf("end : %d \n",end_);
            fprintf("= %d point \n\n",end_ - start + 1);
            break
        end
    end
    
    % len = end_ - start + 1;
    
    z(:,1) = trace.y(start: end_) - 3.2;
    
%      if count == 0 
%          z(:,1) = trace.y(start: end_) - 3.2;
%          count = count + 1;
%      else
%          if length(z(:,1)) < len 
%              z(len,1) = 0;
%              z(:,1) = trace.y(start: end_) - 3.2;
%          else
%              z(:,1) = trace.y(start: end_) - 3.2;
%          end
%      end
     
   
    % 추출한 trace를 파일에 쓸 수 있게 넘겨라 파형 
    writeNPY(z(:,1),fid);
    
    clearvars z; % z제거 

end

fclose(fid); % wirte에 fclose는 지우고 그냥 열어놔 
 
% % Test
% figure;
% plot(z(:,1));


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

function writeNPY(var, fid) % 동일한 파일 이름 넘겨줘 
% function writeNPY(var, filename)
%
% Only writes little endian, fortran (column-major) ordering; only writes
% with NPY version number 1.0.
%
% Always outputs a shape according to matlab's convention, e.g. (10, 1)
% rather than (10,).

% 파일에 쓰는 값 바꿔라 
%shape = size(var);
len = length(var);
dataType = class(var);

fwrite(fid, len, 'uint32');
fwrite(fid, var, dataType);


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

function wave=read_lecroywave(fn,i)

fid=fopen(fn,'r');
if fid==-1 
   disp (sprintf('ERROR: file not found: %s', fn));
   return
end;

%---------------------------------------------------------------------------------------
% Seek offset in the header block
%---------------------------------------------------------------------------------------
data=fread(fid,50);
WAVEDESC=findstr('WAVEDESC', char(data(1:50)'))-1; 
   % subtract 1 because: 
   %		- 1st byte in the File = Index [0]
   %		- 1st byte in the Matlab-Array = Index[1]
   
%---------------------------------------------------------------------------------------
% Define the addresses of the various informations in the file
% These addresses are valid for the template LECROY_2_3 and are subject to change in
% future revisions of the LeCroy firmware. Testing to this template version
% has been disabled to facilitate decoding of waveforms from scopes
% utilizing version 2_2 of the template.
%---------------------------------------------------------------------------------------
TESTED_TEMPLATE = 'LECROY_2_3';

%Addresses (WAVEDESC + address as stated in the LECROY template)
aTEMPLATE_NAME		= WAVEDESC+ 16;
aCOMM_TYPE			= WAVEDESC+ 32;
aCOMM_ORDER			= WAVEDESC+ 34;
aWAVE_DESCRIPTOR	= WAVEDESC+ 36;	% length of the descriptor block
aUSER_TEXT			= WAVEDESC+ 40;	% length of the usertext block
aTRIGTIME_ARRAY     = WAVEDESC+ 48;
aWAVE_ARRAY_1		= WAVEDESC+ 60;	% length (in Byte) of the sample array
aINSTRUMENT_NAME	= WAVEDESC+ 76;
aINSTRUMENT_NUMBER  = WAVEDESC+ 92;
aTRACE_LABEL		= WAVEDESC+ 96;
aWAVE_ARRAY_COUNT	= WAVEDESC+ 116;
aSUBARRAY_COUNT     = WAVEDESC+ 144;
aVERTICAL_GAIN		= WAVEDESC+ 156;
aVERTICAL_OFFSET	= WAVEDESC+ 160;
aNOMINAL_BITS		= WAVEDESC+ 172;
aHORIZ_INTERVAL     = WAVEDESC+ 176;
aHORIZ_OFFSET		= WAVEDESC+ 180;
aVERTUNIT			= WAVEDESC+ 196;
aHORUNIT			= WAVEDESC+ 244;
aTRIGGER_TIME		= WAVEDESC+ 296;
aRECORD_TYPE		= WAVEDESC+ 316;
aPROCESSING_DONE	= WAVEDESC+ 318;
aTIMEBASE			= WAVEDESC+ 324;
aVERT_COUPLING		= WAVEDESC+ 326;
aPROBE_ATT			= WAVEDESC+ 328;
aFIXED_VERT_GAIN	= WAVEDESC+ 332;
aBANDWIDTH_LIMIT	= WAVEDESC+ 334;
aVERTICAL_VERNIER	= WAVEDESC+ 336;
aACQ_VERT_OFFSET	= WAVEDESC+ 340;
aWAVE_SOURCE		= WAVEDESC+ 344;



%---------------------------------------------------------------------------------------
% determine the number storage format HIFIRST / LOFIRST 	(big endian / little endian)
%---------------------------------------------------------------------------------------

fseek(fid,aCOMM_ORDER,'bof');
COMM_ORDER=fread(fid,1,'int16');

fclose(fid);
% reopen the data file using the correct HIFIRST/LOFIRST format
if COMM_ORDER==0    
   fid=fopen(fn,'r','ieee-be');		% HIFIRST
else 
   fid=fopen(fn,'r','ieee-le');		% LOFIRST
end;

%---------------------------------------------------------------------------------------
%  Get the waveform information
%---------------------------------------------------------------------------------------
% Check the template revision  (Commented out to facilitate decoding of 2_2
% files
TEMPLATE_NAME		= ReadString(fid, aTEMPLATE_NAME);
if ~strcmp( deblank(TEMPLATE_NAME), TESTED_TEMPLATE)
  disp (sprintf ('WARNING!\n %s %s %s\n %s %s %s',...
			'This function has been written for the LeCroy-Template',...
			TESTED_TEMPLATE, '.',...
			'The current file contains information created with the template',...
			TEMPLATE_NAME, '.'));
end

% Instrument
wave.info.INSTRUMENT_NAME	= ReadString(fid, aINSTRUMENT_NAME);
wave.info.INSTRUMENT_NUMBER	= ReadLong	(fid, aINSTRUMENT_NUMBER);
wave.info.Filename			= fn;

% Channel
wave.info.TRIGGER_TIME		= ReadTimestamp(fid, aTRIGGER_TIME);

tmp=['channel 1';'channel 2';'channel 3';'channel 4';'unknown  '];
wave.info.WAVE_SOURCE			= tmp (1+ ReadWord(fid, aWAVE_SOURCE),:);

tmp=['DC_50_Ohms'; 'ground    ';'DC 1MOhm  ';'ground    ';'AC 1MOhm  '];
wave.info.VERT_COUPLING		= deblank (tmp (1+ ReadWord(fid, aVERT_COUPLING),:));

tmp=['off'; 'on '];
wave.info.BANDWIDTH_LIMIT	= deblank (tmp (1+ ReadWord(fid, aBANDWIDTH_LIMIT),:));

tmp=[
   'single_sweep      ';	'interleaved       '; 'histogram         ';
   'graph             ';	'filter_coefficient'; 'complex           ';
   'extrema           ';	'sequence_obsolete '; 'centered_RIS      ';	
   'peak_detect       '];
wave.info.RECORD_TYPE		= deblank (tmp (1+ ReadWord(fid, aRECORD_TYPE),:));

tmp=[
   'no_processing';   'fir_filter   '; 'interpolated ';   'sparsed      ';
   'autoscaled   ';   'no_result    '; 'rolling      ';   'cumulative   '];
wave.info.PROCESSING_DONE		= deblank (tmp (1+ ReadWord(fid, aPROCESSING_DONE),:));


% Vertical settings
FIXED_VERT_GAIN			= ReadFixed_vert_gain(fid, aFIXED_VERT_GAIN);
PROBE_ATT				= ReadFloat (fid, aPROBE_ATT);
VERTICAL_GAIN			= ReadFloat	(fid, aVERTICAL_GAIN);
VERTICAL_OFFSET			= ReadFloat	(fid, aVERTICAL_OFFSET);
wave.info.NOMINAL_BITS	= ReadWord	(fid, aNOMINAL_BITS);
wave.info.Gain_with_Probe = strcat (Float_to_Eng(FIXED_VERT_GAIN*PROBE_ATT), 'V/div');

% Horizontal settings
HORIZ_INTERVAL			= ReadFloat(fid, aHORIZ_INTERVAL);
HORIZ_OFFSET			= ReadDouble(fid, aHORIZ_OFFSET);
wave.info.TIMEBASE	= strcat (Float_to_Eng (ReadTimebase(fid,aTIMEBASE)), 's/div');
wave.info.SampleRate	= strcat (Float_to_Eng(1/HORIZ_INTERVAL) , 'S/sec');
wave.desc.Ts			= HORIZ_INTERVAL;
wave.desc.fs			= 1/HORIZ_INTERVAL;

%---------------------------------------------------------------------------------------
% Read samples array (Plain binary ADC values)
%---------------------------------------------------------------------------------------
COMM_TYPE			= ReadWord(fid, aCOMM_TYPE);
WAVE_DESCRIPTOR 	= ReadLong(fid, aWAVE_DESCRIPTOR);
USER_TEXT			= ReadLong(fid, aUSER_TEXT);
WAVE_ARRAY_1		= ReadLong(fid, aWAVE_ARRAY_1);  % 파형 
WAVE_ARRAY_COUNT    = ReadLong(fid, aWAVE_ARRAY_COUNT);
TRIGTIME_ARRAY      = ReadLong(fid, aTRIGTIME_ARRAY);

% returns number of segments in acquisition
wave.info.nbSegments      = ReadLong(fid, aSUBARRAY_COUNT);


if wave.info.nbSegments > 1
    % for sequence mode only
    
    %---------------------------------------------------------------------------------------
    % Read contents of TRIGTIME_ARRAY, which is an interleaved array
    %---------------------------------------------------------------------------------------

    % Take from X-Stream oscilloscopes remote control manual, appendix II:
    % <  0>          TRIGGER_TIME: double     ; for sequence acquisitions, 
    %                                         ; time in seconds from first 
    %                                         ; trigger to this one 
    % <  8>          TRIGGER_OFFSET: double   ; the trigger offset is in seconds 
    %                                         ; from trigger to zeroth data point
    fseek(fid, WAVEDESC + WAVE_DESCRIPTOR + USER_TEXT, 'bof');
    trigtime_array_temp = fread(fid, 2*wave.info.nbSegments, 'double');
    % We need to de-interleave the trigger_time and trigger_offset data.
    wave.trigger_time = trigtime_array_temp(1:2:end);
    wave.trigger_offset = trigtime_array_temp(2:2:end);
    
    if WAVE_ARRAY_1/wave.info.nbSegments ~= WAVE_ARRAY_1/wave.info.nbSegments
        % if this condition happens, MATLAB will pad the rest of the output
        % matrix with zeros, so the file is still readable, but this could
        % signify another error (in this m-file?)
        warning('While reading segment file: Total number of points is not a multiple of the number of segments');
    end
    
    %---------------------------------------------------------------------------------------
    % Read the ADC values into a matrix
    %---------------------------------------------------------------------------------------
    % since this is a sequence acquisition, we will return a
    % (WAVE_ARRAY_COUNT/nbSegments)x(nbSegments) matrix, where each column is one segment.
    fseek(fid, WAVEDESC + WAVE_DESCRIPTOR + USER_TEXT + TRIGTIME_ARRAY, 'bof');
    if COMM_TYPE == 0  % byte
       wave.y = fread(fid, [WAVE_ARRAY_COUNT/wave.info.nbSegments wave.info.nbSegments], 'int8');
        %fprintf("%d \n",WAVE_ARRAY_1);
    else	%	word
       wave.y = fread(fid, [WAVE_ARRAY_COUNT/wave.info.nbSegments wave.info.nbSegments], 'int16');
        %fprintf("%d \n",WAVE_ARRAY_1);
    end;
    
    %---------------------------------------------------------------------------------------
    % Create corresponding matrix of time, with correction for each trigger time
    %---------------------------------------------------------------------------------------
    wave.x = repmat(wave.trigger_time.', WAVE_ARRAY_COUNT/wave.info.nbSegments, 1) + repmat((0:WAVE_ARRAY_COUNT/wave.info.nbSegments-1)'*HORIZ_INTERVAL + HORIZ_OFFSET, 1, wave.info.nbSegments);
    
else

    fseek(fid, WAVEDESC + WAVE_DESCRIPTOR + USER_TEXT + TRIGTIME_ARRAY, 'bof');
    if COMM_TYPE == 0  % byte
       wave.y=fread(fid,WAVE_ARRAY_1, 'int8');
       %fprintf("%d \n",WAVE_ARRAY_1);
    else	%	word
       wave.y=fread(fid,WAVE_ARRAY_1, 'int16');
       fprintf(i +" : %d \n",WAVE_ARRAY_1); %%%%%%%%%%%%동작%%%%%%%%%%%%
    end;

    %---------------------------------------------------------------------------------------
    % Create corresponding array of time
    %---------------------------------------------------------------------------------------
    wave.x = (0:WAVE_ARRAY_COUNT-1)'*HORIZ_INTERVAL + HORIZ_OFFSET;

end

%---------------------------------------------------------------------------------------
% Transform the ADC values to voltages
%---------------------------------------------------------------------------------------
wave.y = VERTICAL_GAIN * wave.y - VERTICAL_OFFSET;

%---------------------------------------------------------------------------------------
% close the waveform file
%---------------------------------------------------------------------------------------
fclose(fid);

%=======================================================================================
% Support functions
%=======================================================================================

%---------------------------------------------------------------------------------------
% Read 8Bit signed Byte (not used)
%---------------------------------------------------------------------------------------
function b=ReadByte(fid, Addr)
	fseek(fid,Addr,'bof');
	b=fread(fid,1,'int8');
   
%---------------------------------------------------------------------------------------
% Read 16Bit signed Word 
%---------------------------------------------------------------------------------------
function w=ReadWord(fid, Addr)
	fseek(fid,Addr,'bof');
	w=fread(fid,1,'int16');
	
%---------------------------------------------------------------------------------------
% Read 32Bit signed Long
%---------------------------------------------------------------------------------------
function l=ReadLong(fid, Addr)
	fseek(fid,Addr,'bof');
	l=fread(fid,1,'int32');
   
%---------------------------------------------------------------------------------------
% Read 32Bit IEEE Float
%---------------------------------------------------------------------------------------
function f=ReadFloat(fid, Addr)
	fseek(fid,Addr,'bof');
	f=fread(fid,1,'float32');
   
%---------------------------------------------------------------------------------------
% Read 64Bit IEEE Double
%---------------------------------------------------------------------------------------
function d=ReadDouble(fid, Addr)
	fseek(fid,Addr,'bof');
	d=fread(fid,1,'float64');
   
%---------------------------------------------------------------------------------------
% Read string (up to 16 characters)
%---------------------------------------------------------------------------------------
function s=ReadString(fid, Addr)
	fseek(fid,Addr,'bof');
	s=fgets(fid,16);

%---------------------------------------------------------------------------------------
% Read timestamp
%---------------------------------------------------------------------------------------
function t=ReadTimestamp(fid, Addr)
	fseek(fid,Addr,'bof');
	seconds	= fread(fid,1,'float64');
   minutes	= fread(fid,1,'int8');
   hours		= fread(fid,1,'int8');
   days		= fread(fid,1,'int8');
   months	= fread(fid,1,'int8');
   year		= fread(fid,1,'int16');
   
   t=sprintf('%i.%i.%i, %i:%i:%2.0f', days, months, year, hours, minutes, seconds);
%---------------------------------------------------------------------------------------
% Timebase aus dem File lesen
%---------------------------------------------------------------------------------------
function t=ReadTimebase(fid, Addr)
	fseek(fid,Addr,'bof');
	e=fread(fid,1,'int16');
   
   tmp=[1 2 5];
   mant = tmp( 1+ mod(e,3));
   ex  = floor (e / 3)-12;
   
   t=mant*10^ex;   
%---------------------------------------------------------------------------------------
% fixed Vertical Gain aus dem File lesen
%---------------------------------------------------------------------------------------
function t=ReadFixed_vert_gain(fid, Addr)
	fseek(fid,Addr,'bof');
	e=fread(fid,1,'int16');
   
   tmp=[1 2 5];
   mant = tmp( 1+ mod(e,3));
   ex  = floor (e / 3)-6;
   
   t= mant*10^ex;   
%---------------------------------------------------------------------------------------
% Transform a Float to the Engineering-Format (returns a string)
%---------------------------------------------------------------------------------------
function s=Float_to_Eng (f)
   ex= floor(log10(f)); 
	exeng=ex-mod(ex,3);
   if exeng<-18; exeng=-18; end
   if exeng>18; exeng=18; end;
   mant=f/10^exeng;
   
   prefix=('afpnum kMGPE');	%prefixes (u=micro, m=milli, k=kilo, ...)
   s=sprintf('%g%s',mant, prefix( (exeng+18)/3 +1));

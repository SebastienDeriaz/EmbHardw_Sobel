-- EmbHardw
-- Sébastien Deriaz
-- 01.12.2021
--
-- Sobel complete custom instruction
library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;
entity sobel is
    port (
        -- Avalon custom interface signals
        Clk    : in std_logic;
        reset  : in std_logic;
        dataa  : in std_logic_vector(31 downto 0);
        datab  : in std_logic_vector(31 downto 0);
        start  : in std_logic;
        result : out std_logic_vector(31 downto 0);
        done   : out std_logic
    );
end sobel;

architecture comp of sobel is
    constant threshold : unsigned(8 downto 0) := '0' & x"7F";
begin
    -- Avalon interfacing
    process (Clk, reset)

        variable P00      : std_logic_vector(7 downto 0); -- x(-1)  y(1)
        variable P01      : std_logic_vector(7 downto 0); -- x(0)   y(2)
        variable P02      : std_logic_vector(7 downto 0); -- x(1)   y(1)
        variable P10      : std_logic_vector(7 downto 0); -- x(-2)  y(0)
        variable P12      : std_logic_vector(7 downto 0); -- x(2)   y(0)
        variable P20      : std_logic_vector(7 downto 0); -- x(-1)  y(-1)
        variable P21      : std_logic_vector(7 downto 0); -- x(0)   y(-2)
        variable P22      : std_logic_vector(7 downto 0); -- x(1)   y(-1)
        variable result_x : signed(15 downto 0);
        variable result_y : signed(15 downto 0);
        variable sum      : unsigned(15 downto 0);
    begin
        P00      := (others => '0');
        P01      := (others => '0');
        P02      := (others => '0');
        P10      := (others => '0');
        P12      := (others => '0');
        P20      := (others => '0');
        P21      := (others => '0');
        P22      := (others => '0');
        result_x := (others => '0');
        result_y := (others => '0');
        sum      := (others => '0');

        if reset = '1' then
            result <= (others => '0');
        elsif rising_edge(Clk) then
            if (start = '1') then
                -- dataa -> P00, P01, P02, P10
                P00      := dataa(31 downto 24);
                P01      := dataa(23 downto 16);
                P02      := dataa(15 downto 8);
                P10      := dataa(7 downto 0);
                -- datab -> P12, P20, P21, P22
                P12      := datab(31 downto 24);
                P20      := datab(23 downto 16);
                P21      := datab(15 downto 8);
                P22      := datab(7 downto 0);

                -- X mask
                result_x := - signed(P00) + signed(P02) - 2 * signed(P10) + 2 * signed(P12) - signed(P20) + signed(P22);
                -- Y mask
                result_y := signed(P00) + 2 * signed(P01) + signed(P02) - signed(P20) - signed(P21) - signed(P22);

                -- absolute value of x and y
                sum      := unsigned(abs(result_x) + abs(result_y));

                if sum > threshold then
                    result <= (others => '1');
                else
                    result <= (others => '0');
                end if;

                done <= '1';
            else
                done <= '0';
            end if;
        end if;
    end process;
end comp;